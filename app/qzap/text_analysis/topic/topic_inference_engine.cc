// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/topic_inference_engine.h"

#include <math.h>
#include <string>
#include <utility>
#include <vector>

#include "thirdparty/gflags/gflags.h"
#include "app/qzap/common/utility/hash.h"
#include "app/qzap/text_analysis/text_miner.pb.h"
#include "app/qzap/text_analysis/topic/base/model.h"
#include "app/qzap/text_analysis/topic/base/vocabulary.h"
#include "app/qzap/text_analysis/topic/inference/explainer.h"
#include "app/qzap/text_analysis/topic/inference/interpreter.h"
#include "app/qzap/text_analysis/topic/inference/multi_chains_gibbs_sampler.h"

DEFINE_int32(peacock_cache_size_mb, 5 * 1024,
             "the cache size for peacock model.");
DEFINE_int32(peacock_num_markov_chains, 5, "the number of markov chains.");
DEFINE_int32(peacock_total_iterations, 15,
             "total iterations for every markov chain.");
DEFINE_int32(peacock_burn_in_iterations, 10,
             "burnin iterations for every markov chain.");
DECLARE_int32(peacock_topic_top_k);
DECLARE_int32(peacock_topic_word_top_k);

namespace qzap {
namespace text_analysis {

bool TopicInferenceEngine::LoadModel(const std::string& model_dir) {
  if (model_.Load(model_dir.c_str()) != 0) {
    LOG(ERROR) << "Load peacock model failed.";
    return false;
  }
  if (!vocab_.Load(model_dir + "/lda.vocab")) {
    LOG(ERROR) << "Load peacock vocabulary failed.";
    return false;
  }
  interpreter_.reset(
      new base::MultiChainsGibbsSampler(model_,
                                        vocab_,
                                        FLAGS_peacock_cache_size_mb,
                                        FLAGS_peacock_num_markov_chains,
                                        FLAGS_peacock_total_iterations,
                                        FLAGS_peacock_burn_in_iterations));
  explainer_.reset(new base::Explainer(model_, vocab_, *interpreter_));

  return true;
}

bool TopicInferenceEngine::InferAndExplain(Document* document) const {
  std::vector<std::string> doc_words;
  for (int32_t i = 0; i < document->bow_token_size(); ++i) {
    for (int32_t j = 0; j < document->bow_token(i).tf(); ++j) {
      doc_words.push_back(document->bow_token(i).text());
    }
  }

  std::vector<std::pair<int32_t, double> > topic_dist;
  std::vector<std::pair<std::string, double> > word_dist;
  explainer_->Explain(doc_words, &topic_dist, &word_dist);

  // record Topics
  double sum = 0.0;
  for (size_t i = 0; i < topic_dist.size(); ++i) {
    Topic* topic = document->add_topic();
    topic->set_id(topic_dist[i].first);
    topic->set_ori_weight(topic_dist[i].second);
    sum += topic_dist[i].second * topic_dist[i].second;
  }
  sum = sqrt(sum);
  // L2-Normalize
  static const double kEpsilon = 1E-6;
  if (sum > kEpsilon) {
    for (int i = 0; i < document->topic_size(); ++i) {
      Topic* topic = document->mutable_topic(i);
      topic->set_weight(topic->ori_weight() / sum);
    }
  }

  // record Topic words
  sum = 0.0;
  for (size_t i = 0; i < word_dist.size(); ++i) {
    TopicWord* topic_word = document->add_topic_word();
    topic_word->set_text(word_dist[i].first);
    topic_word->set_signature(hash_string(topic_word->text()));
    topic_word->set_ori_weight(word_dist[i].second);
    sum += word_dist[i].second * word_dist[i].second;
  }
  sum = sqrt(sum);
  if (sum < kEpsilon) {
    return true;
  }
  for (int i = 0; i < document->topic_word_size(); ++i) {
    TopicWord* topic_word = document->mutable_topic_word(i);
    topic_word->set_weight(topic_word->ori_weight() / sum);
  }

  document->set_has_infered_topic(true);
  document->set_has_explained_topic_word(true);

  return true;
}

}  // namespace text_analysis
}  // namespace qzap

