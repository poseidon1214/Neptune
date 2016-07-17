// Copyright (c) 2013 Tencent Inc.
// Author: YAN Hao (charlieyan@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/inference/sparselda_hill_climber.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/text_analysis/topic/base/common.h"
#include "app/qzap/text_analysis/topic/base/dense_topic_histogram.h"
#include "app/qzap/text_analysis/topic/base/document.h"
#include "app/qzap/text_analysis/topic/base/global_stats.h"
#include "app/qzap/text_analysis/topic/base/model.h"
#include "app/qzap/text_analysis/topic/base/random.h"
#include "app/qzap/text_analysis/topic/base/smoothed_model_cache.h"
#include "app/qzap/text_analysis/topic/base/vocabulary.h"
#include "app/qzap/text_analysis/topic/base/word_stats.h"

namespace qzap {
namespace text_analysis {
namespace base {

void SparseLDAHillClimber::Interpret(
    const std::vector<std::string>& doc_tokens,
    std::vector<std::pair<int32_t, double> >* topic_dist) const {
  base::MTRandom random;
  random.SeedRNG(GenerateRandomSeed(doc_tokens));

  InterpretOneTrail(doc_tokens, topic_dist, &random);
  std::sort(topic_dist->begin(), topic_dist->end(), CompareByProb);
}

void SparseLDAHillClimber::InitializeCache(int64_t cache_size_mb) {
  smoothed_model_cache_.reset(new SmoothedModelCache);
  smoothed_model_cache_->Compute(*model_, cache_size_mb);
  CacheWordTopicPriorMax();
}

void SparseLDAHillClimber::CacheWordTopicPriorMax() {
  smoothing_only_max_.clear();

  for (WordStats::ConstIterator iter(&(model_->GetWordStats()));
       !iter.Done(); iter.Next()) {
    int32_t word = iter.Word();
    double max_prob = -1.0;
    int32_t new_topic = -1;

    const DoubleVector* smoothing_only_bucket =
        smoothed_model_cache_->GetProbWordGivenTopic(word);
    if (smoothing_only_bucket != NULL) {
      for (uint32_t topic = 0; topic < smoothing_only_bucket->size(); ++topic) {
        double prob = model_->TopicPrior(topic) *
            (*smoothing_only_bucket)[topic];
        if (prob > max_prob) {
          max_prob = prob;
          new_topic = topic;
        }
      }
    } else {
      std::pair<int32_t, double> topic_prob =
          model_->GetMaxProbWordGivenTopic(word);
      if (topic_prob.second > max_prob) {
        max_prob = topic_prob.second;
        new_topic = topic_prob.first;
      }
    }

    smoothing_only_max_[word] = std::make_pair(new_topic, max_prob);
  }
}

void SparseLDAHillClimber::InterpretOneTrail(
    const std::vector<std::string>& doc_tokens,
    std::vector<std::pair<int32_t, double> >* topic_dist,
    base::Random* random) const {
  Document doc(model_->NumTopics());
  doc.ParseFromTokens(doc_tokens, *vocab_, *model_, random);

  for (int32_t i = 0; i < total_iterations_; ++i) {
    for (Document::Iterator it(&doc); !it.Done(); it.Next()) {
      const int32_t word = it.Word();
      const int32_t old_topic = it.Topic();

      // --
      doc.DecrementTopicHistogram(old_topic, 1);

      // ++
      const int32_t new_topic = SampleNewTopic(&doc, word);
      it.SetTopic(new_topic);
      doc.IncrementTopicHistogram(new_topic, 1);
    }
  }

  UnifyDistribution(*doc.GetConstTopicHistogram(), topic_dist);
}

int32_t SparseLDAHillClimber::SampleNewTopic(Document* doc,
                                             int32_t word) const {
  const DenseTopicHistogram& global_histogram =
      model_->GetGlobalTopicHistogram();
  const WordTopicHistogram& word_histogram =
      model_->GetWordTopicHistogram(word);
  const DocumentTopicHistogram* doc_histogram = doc->GetTopicHistogram();

  std::tr1::unordered_map<int32_t, std::pair<int32_t, double> >::const_iterator
      prior_max_iter = smoothing_only_max_.find(word);
  CHECK(prior_max_iter != smoothing_only_max_.end()) <<
      "word not found in prior-max cache: " << word;

  int32_t new_topic = prior_max_iter->second.first;
  double max_prob = prior_max_iter->second.second;
  double topic_word_factor = 0.0;

  const DoubleVector* word_dist =
      smoothed_model_cache_->GetProbWordGivenTopic(word);
  if (word_dist != NULL) {
    for (DocumentTopicHistogram::ConstIterator topic_iter(*doc_histogram);
         !topic_iter.Done(); topic_iter.Next()) {
      const int32_t topic = topic_iter.Topic();
      topic_word_factor = (*word_dist)[topic];
      double prob = (topic_iter.Count() + model_->TopicPrior(topic)) *
          topic_word_factor;
      if (prob > max_prob) {
        max_prob = prob;
        new_topic = topic;
      }
    }
  } else {
    for (DocumentTopicHistogram::ConstIterator topic_iter(*doc_histogram);
         !topic_iter.Done(); topic_iter.Next()) {
      const int32_t topic = topic_iter.Topic();
      topic_word_factor = (word_histogram.Count(topic) + model_->WordPrior()) /
          (model_->WordPriorSum() + global_histogram[topic]);
      double prob = (topic_iter.Count() + model_->TopicPrior(topic)) *
          topic_word_factor;
      if (prob > max_prob) {
        max_prob = prob;
        new_topic = topic;
      }
    }
  }

  return new_topic;
}

void SparseLDAHillClimber::UnifyDistribution(
    const OrderedSparseHistogram& topic_hist,
    std::vector<std::pair<int32_t, double> >* topic_dist) const {
  topic_dist->resize(topic_hist.Length());
  double sum = model_->TopicPriorSum();

  for (OrderedSparseHistogram::ConstIterator citer(topic_hist);
       !citer.Done(); citer.Next()) {
    sum += citer.Count();
  }

  if (sum > 0) {
    int32_t i = 0;
    for (OrderedSparseHistogram::ConstIterator citer(topic_hist);
         !citer.Done(); citer.Next()) {
      (*topic_dist)[i++] =
          std::make_pair(
              citer.Topic(),
              (citer.Count() + model_->TopicPrior(citer.Topic())) / sum);
    }
  }
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

