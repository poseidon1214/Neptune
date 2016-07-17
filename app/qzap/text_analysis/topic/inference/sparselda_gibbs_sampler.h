// Copyright (c) 2013 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Zhihui JIN (rickjin@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)
//         Zhenlong Sun (richardsun@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_SPARSELDA_GIBBS_SAMPLER_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_SPARSELDA_GIBBS_SAMPLER_H_

#include <stdint.h>
#include <tr1/unordered_map>
#include <utility>
#include <vector>

#include "app/qzap/common/base/scoped_ptr.h"
#include "thirdparty/glog/logging.h"
#include "app/qzap/text_analysis/topic/base/smoothed_model_cache.h"
#include "app/qzap/text_analysis/topic/inference/interpreter.h"

namespace qzap {
namespace text_analysis {
namespace base {

class Random;
class Document;
class Model;
class Vocabulary;

// SparseLDAGibbsSampler implements SparseLDA gibbs sampling algorithm for
// LDA inference. In Gibbs sampling formula:
//
// p(z|w) --> p(z|d) * p(w|z) --> (\alpha_z + N_z|d) * p(w|z)
//        --> \alpha_z * p(w|z) + N_z|d * p(w|z)
//                  s(z,w)        r(z,w,d)
//
// This process divides the full sampling mass into two buckets, where
// s(z,w) is a smoothing-only bucket, r(z,w,d) is a document-topic bucket.
//
// To achieve time- and memory-efficiency when using the large scale LDA model,
// such as 300k words and 1M topics, topic distributions p(w|z) of frequent
// words are pre-computed and cached.
//
// For more details, pls refer to paper:
//  Limin Yao, David Mimno, and Andrew McCallum. Efficient Methods for
//  Topic Model Inference on Streaming Document Collections. KDD'2009.
class SparseLDAGibbsSampler : public Interpreter {
 public:
  SparseLDAGibbsSampler(const Model& model,
                        const Vocabulary& vocab,
                        int64_t cache_size_mb,
                        int32_t total_iterations,
                        int32_t burn_in_iterations)
      : Interpreter(model, vocab, total_iterations),
        burn_in_iterations_(burn_in_iterations) {
    CHECK_LT(0, burn_in_iterations_);
    CHECK_LT(burn_in_iterations_, total_iterations_);
    InitializeCache(cache_size_mb);
  }

  virtual ~SparseLDAGibbsSampler() {}

  // Interpret use a trained model to interpret topics embedded in the given
  // document doc_tokens. This function returns topics sorted by their
  // probabilities.
  virtual void Interpret(
      const std::vector<std::string/*word*/>& doc_tokens,
      std::vector<std::pair<int32_t/*topic*/, double/*P_t|W*/> >* topic_dist)
      const;

 protected:
  // Words in vocab_.GetOrderedWordList() are sorted by frequency in descending
  // order. While initializing cache, the words' topic distributions are cached
  // word by word in the same order, till memory size meets the limit.
  void InitializeCache(int64_t cache_size_mb/* in MB */);

  void CacheWordTopicPriorSum();

  // InterpretOneChain performs one markov chain.
  // Itertaion counting is one-based, the topic distribution in iterations
  // after burn_in_iterations_ will be accumulated, and returns the average
  // topic distributions.
  void InterpretOneChain(
      const std::vector<std::string/*word*/>& doc_tokens,
      std::vector<std::pair<int32_t/*topic*/, double/*P_t|W*/> >* topic_dist,
      DoubleMatrix* word_topic_dists_cache,
      Random* random) const;

  int32_t SampleNewTopic(const Document& doc,
                         int32_t word,
                         const DoubleVector& smoothing_only_bucket,
                         Random* random) const;

  double CalculateDocumentTopicBucket(
      const Document& doc,
      int32_t word,
      const DoubleVector& smoothing_only_bucket,
      std::vector<std::pair<int32_t, double> >* doc_topic_bucket) const;

  // Converting the topic histogram (counting distribution) to
  // topic distribution (probability distribution)
  virtual void UnifyDistribution(
      const OrderedSparseHistogram& accumulated_topic_hist,
      std::vector<std::pair<int32_t/*topic*/, double/*P_t|d*/> >* topic_dist)
      const;

  int32_t burn_in_iterations_;

  scoped_ptr<SmoothedModelCache> smoothed_model_cache_;

  std::tr1::unordered_map<int32_t/*word*/, double> smoothing_only_sum_;
};

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_SPARSELDA_GIBBS_SAMPLER_H_

