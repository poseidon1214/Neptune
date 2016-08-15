// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Qian Wang <cernwang@tencent.com>

#ifndef LEARNING_TUNING_TUNER_H_
#define LEARNING_TUNING_TUNER_H_

#include <string>
#include <list>
#include "thirdparty/glog/logging.h"


namespace gdt {
namespace learning {

// 自动化调参工具
template <class Model, class DataContainer, class Parameter, class Evaluter>
class Tunner {
  public:
    // 初始化
    bool Init(const TunnerConfig& conf) {
      return true;
    }
    // 获取历史结果
    bool Save();
    // 载入历史结果
    bool Load();
    // Refernce: http://scikit-learn.org/stable/modules/grid_search.html
    bool GridSearch(const Data& data, Parameter* parameter);

  private:
  	// 历史实验
  	std::vector<Experiment> experiment;
};

}  // namespace learning
}  // namespace gdt

#endif  // LEARNING_TUNING_TUNER_H_
