// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_INTERPERTER_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_INTERPERTER_H_

#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include "thirdparty/glog/logging.h"

namespace qzap {
namespace text_analysis {
namespace base {

class Document;
class Model;
class OrderedSparseHistogram;
class Vocabulary;

// Interpreter is the interface to Peacock/LDA inference algorithm.
class Interpreter {
 public:
  Interpreter(const Model& model,
              const Vocabulary& vocab,
              int32_t total_iterations)
      : model_(&model),
        vocab_(&vocab),
        total_iterations_(total_iterations) {
    CHECK_LT(0, total_iterations_);
  }

  virtual ~Interpreter() {}

  // Interpret use a trained model to interpret topics embedded in the given
  // document doc_words. This function returns topics with their probabilities.
  // Inference algorithms usually need configuration parameters, which are
  // recommended to be passed in through the constructor of derived class;
  // instead of changing this function signiture.
  virtual void Interpret(
      const std::vector<std::string>/*word*/& doc_words,
      std::vector<std::pair<int32_t/*topic*/, double/*P_t|W*/> >* topic_dist)
      const = 0;

 protected:
  // Converting the topic histogram (counting distribution) to
  // topic distribution (probability distribution)
  virtual void UnifyDistribution(
      const OrderedSparseHistogram& topic_hist,
      std::vector<std::pair<int32_t/*topic*/, double/*P_t|d*/> >* topic_dist)
      const = 0;

  const Model* model_;
  const Vocabulary* vocab_;
  int32_t total_iterations_;
};

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_INTERPERTER_H_

