// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
// optimized MaxEnt predictor

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_OPTIM_MAXENT_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_OPTIM_MAXENT_H_

#include "thirdparty/google/dense_hash_map"
#include "app/qzap/text_analysis/classifier/classifier_base.h"

namespace qzap {
namespace text_analysis {

class OptimMaxEnt : public ClassifierBase {
 public:
  OptimMaxEnt();
  virtual ~OptimMaxEnt();

  virtual bool LoadFromDir(const std::string& dir);

  virtual void Predict(const Instance& instance, Result* result) const;

 private:
  bool LoadWeights(const std::string& filepath);

  void CalculateProbabilities(const Instance& instance,
                              std::vector<double>* prob) const;

  typedef google::dense_hash_map<int32_t /* category-id */,
          google::dense_hash_map<int32_t /* feature-id */, double> >
              SparseMatrix;

  SparseMatrix weights_;
};  // class OptimMaxEnt

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_OPTIM_MAXENT_H_
