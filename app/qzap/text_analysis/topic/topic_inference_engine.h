// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// Topic Word Explainer.

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_TOPIC_WORD_EXPLAINER_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_TOPIC_WORD_EXPLAINER_H_

#include <string>

#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/topic/base/model.h"
#include "app/qzap/text_analysis/topic/base/vocabulary.h"
#include "app/qzap/text_analysis/topic/inference/explainer.h"
#include "app/qzap/text_analysis/topic/inference/interpreter.h"

namespace qzap {
namespace text_analysis {

class Document;

namespace base {
class Model;
class Vocabulary;
class Interpreter;
class Explainer;
}  // namespace base

class TopicInferenceEngine {
 public:
  TopicInferenceEngine() : model_(0) {}
  ~TopicInferenceEngine() {}

  bool LoadModel(const std::string& model_dir);

  bool InferAndExplain(Document* document) const;

 private:
  base::Model model_;
  base::Vocabulary vocab_;
  scoped_ptr<base::Interpreter> interpreter_;
  scoped_ptr<base::Explainer> explainer_;

  DECLARE_UNCOPYABLE(TopicInferenceEngine);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_TOPIC_WORD_EXPLAINER_H_

