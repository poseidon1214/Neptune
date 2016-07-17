// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_CLASSIFIER_BASE_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_CLASSIFIER_BASE_H_

#include <string>
#include <vector>
#include "app/qzap/text_analysis/classifier/classifier.pb.h"
#include "app/qzap/text_analysis/classifier/instance.h"
#include "app/qzap/text_analysis/classifier/taxonomy_hierarchy.h"

namespace qzap {
namespace text_analysis {

inline bool CategoryProbabilityGreater(const Label& lhs, const Label& rhs) {
  return lhs.probability() > rhs.probability();
}

class ClassifierBase {
 public:
  typedef std::vector<std::vector<Label> > Result;

  virtual ~ClassifierBase() {}

  virtual bool LoadFromDir(const std::string& dir) = 0;

  virtual void Predict(const Instance& instance, Result* result) const = 0;

  const TaxonomyHierarchy& taxonomy() const { return taxonomy_; }

 protected:
  TaxonomyHierarchy taxonomy_;
};  // class ClassifierBase

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_CLASSIFIER_BASE_H_
