// Copyright (c) 2013 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Zhihui JIN (rickjin@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)
//         Zhenlong Sun (richardsun@tencent.com)

#include "app/qzap/text_analysis/topic/inference/sparselda_gibbs_sampler.h"

#include <algorithm>
#include <string>
#include <tr1/unordered_set>
#include <vector>

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

void SparseLDAGibbsSampler::Interpret(
    const std::vector<std::string>& doc_tokens,
    std::vector<std::pair<int32_t, double> >* topic_dist) const {
  base::MTRandom random;
  random.SeedRNG(GenerateRandomSeed(doc_tokens));
  DoubleMatrix word_topic_dists_cache;

  InterpretOneChain(doc_tokens, topic_dist, &word_topic_dists_cache, &random);
  std::sort(topic_dist->begin(), topic_dist->end(), CompareByProb);
}

void SparseLDAGibbsSampler::InitializeCache(int64_t cache_size_mb) {
  smoothed_model_cache_.reset(new SmoothedModelCache);
  smoothed_model_cache_->Compute(*model_, cache_size_mb);
  CacheWordTopicPriorSum();
}

void SparseLDAGibbsSampler::CacheWordTopicPriorSum() {
  smoothing_only_sum_.clear();

  for (WordStats::ConstIterator iter(&(model_->GetWordStats()));
       !iter.Done(); iter.Next()) {
    int32_t word = iter.Word();
    double sum = 0.0;

    const DoubleVector* smoothing_only_bucket =
        smoothed_model_cache_->GetProbWordGivenTopic(word);
    if (smoothing_only_bucket != NULL) {
      for (uint32_t topic = 0; topic < smoothing_only_bucket->size(); ++topic) {
        sum += model_->TopicPrior(topic) * (*smoothing_only_bucket)[topic];
      }
    } else {
      DoubleVector word_dist;
      model_->GetProbWordGivenTopic(word, &word_dist);
      for (uint32_t topic = 0; topic < word_dist.size(); ++topic) {
        sum += model_->TopicPrior(topic) * word_dist[topic];
      }
    }

    smoothing_only_sum_[word] = sum;
  }
}

void SparseLDAGibbsSampler::InterpretOneChain(
    const std::vector<std::string>& doc_tokens,
    std::vector<std::pair<int32_t, double> >* topic_dist,
    DoubleMatrix* word_topic_dists_cache,
    base::Random* random) const {
  Document doc(model_->NumTopics());
  doc.ParseFromTokens(doc_tokens, *vocab_, *model_, random);

  OrderedSparseHistogram accumulated_topic_hist(model_->NumTopics());
  for (int32_t i = 1; i <= total_iterations_; ++i) {
    // iterator
    for (Document::Iterator it(&doc); !it.Done(); it.Next()) {
      const int32_t word = it.Word();
      const int32_t old_topic = it.Topic();

      // --
      doc.DecrementTopicHistogram(old_topic, 1);

      const DoubleVector* smoothing_only_bucket =
          smoothed_model_cache_->GetProbWordGivenTopic(word);
      if (smoothing_only_bucket == NULL) {
        if (word_topic_dists_cache->find(word) ==
            word_topic_dists_cache->end()) {
          model_->GetProbWordGivenTopic(word,
                                        &(*word_topic_dists_cache)[word]);
        }
        smoothing_only_bucket = &(*word_topic_dists_cache)[word];
      }

      const int32_t new_topic = SampleNewTopic(doc, word,
                                               *smoothing_only_bucket, random);
      it.SetTopic(new_topic);
      // ++
      doc.IncrementTopicHistogram(new_topic, 1);
    }

    if (i > burn_in_iterations_) {
      DocumentTopicHistogram::ConstIterator citer(
          *doc.GetConstTopicHistogram());
      while (!citer.Done()) {
        accumulated_topic_hist.IncrementTopic(citer.Topic(), citer.Count());
        citer.Next();
      }
    }
  }

  UnifyDistribution(accumulated_topic_hist, topic_dist);
}

int32_t SparseLDAGibbsSampler::SampleNewTopic(
    const Document& doc, int32_t word,
    const DoubleVector& smoothing_only_bucket,
    base::Random* random) const {
  std::vector<std::pair<int32_t, double> > doc_topic_bucket;

  double doc_topic_sum = CalculateDocumentTopicBucket(doc,
                                                      word,
                                                      smoothing_only_bucket,
                                                      &doc_topic_bucket);
  double sum = doc_topic_sum + smoothing_only_sum_.find(word)->second;
  double sample = random->RandDouble() * sum;

  int32_t new_topic = kUnassignedTopic;
  if (sample < doc_topic_sum) {  // sample in document topic bucket
    for (size_t i = 0; i < doc_topic_bucket.size(); ++i) {
      sample -= doc_topic_bucket[i].second;
      if (sample <= 0) {
        new_topic = doc_topic_bucket[i].first;
        break;
      }
    }
    CHECK(sample <= 0);
  } else {  // sample in smoothing only bucket
    sample -= doc_topic_sum;
    int32_t i = 0;
    sample -= smoothing_only_bucket[i] * model_->TopicPrior(i);
    while (sample > 0) {
      ++i;
      sample -= smoothing_only_bucket[i] * model_->TopicPrior(i);
    }
    CHECK(i < model_->NumTopics());
    new_topic = i;
  }

  CHECK(new_topic != kUnassignedTopic);
  return new_topic;
}

double SparseLDAGibbsSampler::CalculateDocumentTopicBucket(
    const Document& doc,
    int32_t word,
    const DoubleVector& smoothing_only_bucket,
    std::vector<std::pair<int32_t, double> >* doc_topic_bucket) const {
  double doc_topic_sum = 0.0;

  DocumentTopicHistogram::ConstIterator topic_iter(
      *doc.GetConstTopicHistogram());

  for (; !topic_iter.Done(); topic_iter.Next()) {
    const int32_t topic = topic_iter.Topic();
    doc_topic_bucket->push_back(
        std::make_pair(topic,
                       topic_iter.Count() * smoothing_only_bucket[topic]));
    doc_topic_sum += doc_topic_bucket->back().second;
  }

  return doc_topic_sum;
}

void SparseLDAGibbsSampler::UnifyDistribution(
    const OrderedSparseHistogram& accumulated_topic_hist,
    std::vector<std::pair<int32_t, double> >* topic_dist) const {
  topic_dist->resize(accumulated_topic_hist.Length());
  double sum = model_->TopicPriorSum();

  for (OrderedSparseHistogram::ConstIterator citer(accumulated_topic_hist);
       !citer.Done(); citer.Next()) {
    sum += 1.0 * citer.Count() / (total_iterations_ - burn_in_iterations_);
  }

  if (sum > 0) {
    int32_t i = 0;
    for (OrderedSparseHistogram::ConstIterator citer(accumulated_topic_hist);
         !citer.Done(); citer.Next()) {
      (*topic_dist)[i++] =
          std::make_pair(
              citer.Topic(),
              (1.0 * citer.Count() / (total_iterations_ - burn_in_iterations_) +
               model_->TopicPrior(citer.Topic())) / sum);
    }
  }
}
}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

