// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_MULTI_CHAINS_GIBBS_SAMPLER_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_MULTI_CHAINS_GIBBS_SAMPLER_H_

#include <utility>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/text_analysis/topic/inference/sparselda_gibbs_sampler.h"

namespace qzap {
namespace text_analysis {
namespace base {

class Document;
class Vocabulary;
class Model;

// MultiChainsGibbsSampler implements multi-markov-chain based SparseLDA gibbs
// sampling algorithm for LDA inference.
//
// For more details, pls refer to paper:
//  Xing-Wei and W.Bruce Croft. LDA-Based Document Models for Ad-hoc Retrieval.
//  SIGIR'2006
class MultiChainsGibbsSampler : public SparseLDAGibbsSampler {
 public:
  MultiChainsGibbsSampler(const Model& model,
                         const Vocabulary& vocab,
                         int64_t cache_size_mb,
                         int32_t num_markov_chains,
                         int32_t total_iterations,
                         int32_t burn_in_iterations)
      : SparseLDAGibbsSampler(model,
                              vocab,
                              cache_size_mb,
                              total_iterations,
                              burn_in_iterations),
        num_markov_chains_(num_markov_chains) {
    CHECK_LT(0, num_markov_chains_);
  }

  virtual ~MultiChainsGibbsSampler() {}

  // Interpret use a trained model to interpret topics embedded in the given
  // document doc_tokens. This function returns topics sorted by their
  // probabilities.
  virtual void Interpret(
      const std::vector<std::string/*word*/>& doc_tokens,
      std::vector<std::pair<int32_t/*topic*/, double/*P_t|W*/> >* topic_dist)
      const;

 private:
  int32_t num_markov_chains_;
};

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_MULTI_CHAINS_GIBBS_SAMPLER_H_

