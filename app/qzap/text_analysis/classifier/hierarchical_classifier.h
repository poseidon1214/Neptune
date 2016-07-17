// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_HIERARCHICAL_CLASSIFIER_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_HIERARCHICAL_CLASSIFIER_H_

#include <tr1/memory>
#include <tr1/unordered_map>
#include "app/qzap/text_analysis/classifier/classifier_base.h"

namespace qzap {
namespace text_analysis {

class HierarchicalClassifier : public ClassifierBase {
 public:
  virtual bool LoadFromDir(const std::string& dir);

  virtual void Predict(const Instance& instance, Result* result) const;

 private:
  void PredictAux(const Instance& instance, int32_t id,
                  double continue_threshold, Result* result) const;

  typedef std::tr1::unordered_map<int32_t,
      std::tr1::shared_ptr<ClassifierBase> > ClassifierMap;

  ClassifierMap classifiers_;
};  // class HierarchicalClassifier

}  // namespace classifier
}  // namespace paraglo

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_HIERARCHICAL_CLASSIFIER_H_
