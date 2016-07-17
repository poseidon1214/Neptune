// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_MAXENT_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_MAXENT_H_

#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/classifier/classifier_base.h"

namespace maxent {
class ME_Model;
class ME_Sample;
}  // namespace maxent

namespace qzap {
namespace text_analysis {

typedef maxent::ME_Model MaxEntImpl;
typedef maxent::ME_Sample MaxEntSample;

class MaxEnt : public ClassifierBase {
 public:
  MaxEnt();
  virtual ~MaxEnt();

  virtual bool LoadFromDir(const std::string& dir);

  virtual void Predict(const Instance& instance, Result* result) const;

 private:
  MaxEntImpl* impl_;

  DECLARE_UNCOPYABLE(MaxEnt);
};  // class MaxEnt

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_MAXENT_H_
