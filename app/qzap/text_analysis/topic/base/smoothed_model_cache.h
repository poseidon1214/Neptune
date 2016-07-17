// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_SMOOTHED_MODEL_CACHE_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_SMOOTHED_MODEL_CACHE_H_

#include "app/qzap/text_analysis/topic/base/common.h"

namespace qzap {
namespace text_analysis {
namespace base {

class Model;
class Vocabulary;

// SmoothedModelCache implements memory-efficient cache of SparseLDA sampling
// algorithm for LDA. In Gibbs sampling formula:
//
// p(z|w) --> p(z|d) * p(w|z) --> (\alpha_z + N_z|d) * p(w|z)
//        --> \alpha_z * p(w|z) + N_z|d * p(w|z)
//                s(z,w)             r(z,w,d)
//
// p(w|z) --> (\beta + N_w|z) / (\beta * V + N_z)
//
// This process divides the full sampling mass into two buckets, where
// s(z,w) is a smoothing-only bucket, r(z,w,d) is a document-topic bucket.
// Note that p(w|z) are document independent parameters provided by the model
// during inference phase, thus can be pre-computed and cached in memory
// in advance.  Usually we do not cache the whole vocabulary for
// that may cost too much memory.
//
// For more details, pls refer to paper:
//  Limin Yao, David Mimno, and Andrew McCallum. Efficient Methods for
//  Topic Model Inference on Streaming Document Collections. KDD'2009.
class SmoothedModelCache {
 public:
  SmoothedModelCache() {}
  ~SmoothedModelCache() {}

  // While initializing cache, the frequent words' topic distributions are
  // cached word by word, till memory size meets the limit.
  void Compute(const Model& model, int64_t cache_size_mb/* in MB */);

  // Get a row of the p(w|z) matrix, indexed by word
  // returns NULL when the word is not cached.
  const DoubleVector* GetProbWordGivenTopic(int32_t word) const;

 private:
  DoubleMatrix smoothing_only_matrix_;  // cache p(w|z) of frequent words
};

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_SMOOTHED_MODEL_H_

