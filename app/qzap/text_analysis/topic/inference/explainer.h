// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_EXPLAINER_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_EXPLAINER_H_

#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include "app/qzap/text_analysis/topic/base/common.h"

namespace qzap {
namespace text_analysis {
namespace base {

class Model;
class Vocabulary;
class Interpreter;

// Explainer do Peacock/LDA inference on documents.
class Explainer {
 public:
  Explainer(const Model& model,
            const Vocabulary& vocab,
            const Interpreter& interpreter);

  ~Explainer() {}

  // Explain use an Interpreter to interpret descriptive topic words embedded
  // in the given document, which is represented as a sequence of words.
  // This function returns topic words sorted by their probabilities.
  void Explain(const std::vector<std::string/*word*/>& doc_words,
               SparseDoubleVector* topic_dist,
               SparseStringVector* word_dist) const;

 private:
  // Compute topic word distributions base on topic distribution.
  // P(w|W) = \sum_t p(w|t)p(t|W)
  void GetTopicWords(const SparseDoubleVector& topic_dist,
                     SparseStringVector* word_dist) const;

  // Cached sparse matrix P(w|z) indexed by topic id.
  SparseDoubleMatrix topic_word_dist_;

  const Model& model_;

  const Vocabulary& vocab_;

  const Interpreter& interpreter_;
};

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_EXPLAINER_H_

