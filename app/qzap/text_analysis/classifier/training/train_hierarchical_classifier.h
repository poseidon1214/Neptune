// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//
// Trainer class of hierarchical classifier, whose node-classifier
// is MaxEnt classifier.
//

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TRAINING_TRAIN_HIERARCHICAL_CLASSIFIER_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TRAINING_TRAIN_HIERARCHICAL_CLASSIFIER_H_

#include <tr1/memory>
#include <tr1/unordered_map>
#include "app/qzap/text_analysis/classifier/training/train_classifier_base.h"

namespace qzap {
namespace text_analysis {

class TrainHierarchicalClassifier : public TrainClassifierBase {
 public:
  TrainHierarchicalClassifier();
  virtual ~TrainHierarchicalClassifier();

  virtual void SaveToDir(const std::string& dir) const;

  virtual void Train(const std::string& sample_filepath,
                     const TaxonomyHierarchy& taxonomy);

 private:
  typedef std::tr1::unordered_map<int32_t,
          std::tr1::shared_ptr<TrainClassifierBase> > ClassifierMap;

  // Aux = Auxilian
  void TrainAux(const std::string& sample_filepath, int32_t category_id);

  ClassifierMap classifiers_;
};  // class TrainMaxEnt

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TRAINING_TRAIN_HIERARCHICAL_CLASSIFIER_H_
