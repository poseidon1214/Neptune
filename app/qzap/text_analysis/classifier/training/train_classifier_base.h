// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)
//
// interface for classifier training class
//

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TRAINING_TRAIN_CLASSIFIER_BASE_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TRAINING_TRAIN_CLASSIFIER_BASE_H_

#include <string>
#include <vector>
#include "app/qzap/text_analysis/classifier/instance.h"
#include "app/qzap/text_analysis/classifier/taxonomy_hierarchy.h"

namespace qzap {
namespace text_analysis {

class TrainClassifierBase {
 public:
  virtual ~TrainClassifierBase() {}

  virtual void SaveToDir(const std::string& dir) const = 0;

  // format of training sample file:
  // category\tfeature\tcomments\n
  //
  // format of category-str:
  // level-1-category.level-2-category....level-n-category
  //
  // format of feature-str:
  // id:value id:value ...
  //
  // eg.
  // 游戏.小游戏.QQ小游戏\t0:0.1 10:0.9\t我喜欢玩游戏.
  // 游戏.大游戏.QQ大游戏\t0:0.1 10:0.5 20:0.4\t玩游戏真无聊啊!
  // 家居.时尚家居\t1:0.5 100:0.5\t买家居到宜家;宜家欢迎你.
  // 家居.时尚家居\t\t
  // ...
  //
  // Note:
  // Features MUST be normalized.
  virtual void Train(const std::string& sample_filepath,
                     const TaxonomyHierarchy& taxonomy) = 0;

  const TaxonomyHierarchy& taxonomy() const {
    return taxonomy_;
  }

  // format of training sample string:
  // pls refer to comments of function Train.
  static bool ParseTrainSampleString(const std::string& str,
                                     std::vector<std::string>* category,
                                     Instance* feature,
                                     std::string* comments);

 protected:
  TaxonomyHierarchy taxonomy_;
};  // class ClassifierBase

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TRAINING_TRAIN_CLASSIFIER_BASE_H_
