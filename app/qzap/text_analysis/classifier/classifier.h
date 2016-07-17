// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         YiXiang Mu (javiermu@tencent.com)
//         Pan Yang (baileyyang@tenceng.com)
//         Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_CLASSIFIER_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_CLASSIFIER_H_

#include <string>

#include "app/qzap/common/base/scoped_ptr.h"
#include "common/base/uncopyable.h"

namespace qzap {
namespace text_analysis {

class HierarchicalClassifier;
class Instance;

class Document;
class FeatureExtractor;

class Classifier {
 public:
  Classifier();
  ~Classifier();

  bool LoadModel(const std::string& model_dir);

  // 输入document已经抽取了LDA topic以及bow_tokens等feature，并分别作了L1归一化
  // 在函数里面不做处理
  bool Predict(Document* document) const;

 private:
  void ExtractFeatures(const Document& document, Instance* instance) const;

  scoped_ptr<HierarchicalClassifier> classifier_;
  scoped_ptr<FeatureExtractor> feature_extractor_;

  DECLARE_UNCOPYABLE(Classifier);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_CLASSIFIER_H_

