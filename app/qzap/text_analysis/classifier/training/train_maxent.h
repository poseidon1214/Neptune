// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//
// trainer class of MaxEnt classifier
//
// Core of the training algorithm, pls refer to:
// "A simple C++ library for maximum entropy classification"(
// http://www-tsujii.is.s.u-tokyo.ac.jp/~tsuruoka/maxent/).
// Class TrainMaxEnt is just a wrapper of class ME_Model.
//

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TRAINING_TRAIN_MAXENT_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TRAINING_TRAIN_MAXENT_H_

#include "thirdparty/gflags/gflags.h"
#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/classifier/training/train_classifier_base.h"

DECLARE_int32(maxent_traing_heldout);
DECLARE_string(maxent_training_regularizer_type);
DECLARE_double(maxent_training_l1_regularizer);
DECLARE_double(maxent_training_l2_regularizer);
DECLARE_string(maxent_traing_optim_method);
DECLARE_int32(maxent_traing_sgd_optim_iterations);

namespace maxent {
class ME_Model;
class ME_Sample;
}  // namespace maxent

namespace qzap {
namespace text_analysis {

typedef maxent::ME_Model MaxEntImpl;
typedef maxent::ME_Sample MaxEntSample;

class TrainMaxEnt : public TrainClassifierBase {
 public:
  TrainMaxEnt();
  virtual ~TrainMaxEnt();

  virtual void SaveToDir(const std::string& dir) const;

  virtual void Train(const std::string& sample_filepath,
                     const TaxonomyHierarchy& taxonomy);

 private:
  MaxEntImpl* impl_;

  DECLARE_UNCOPYABLE(TrainMaxEnt);
};  // class TrainMaxEnt

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TRAINING_TRAIN_MAXENT_H_

