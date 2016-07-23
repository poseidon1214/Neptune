// Copyright (c) 2015 Tencent Inc.
// Author: Wang Qian (cernwang@tencent.com)

#ifndef COMMON_STATE_MACHINE_TASK_H_
#define COMMON_STATE_MACHINE_TASK_H_

#include <map>
#include <utility>
#include <vector>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

#include "app/qzap/common/base/base.h"
#include "framework/common/base_processor.h"

namespace gdt {
namespace common {

#define CHECK(result)  \
  if (!(result)) { \
    return false; \
  }

typedef BitMap FinalStatus;

template <class ConfigType, class DataMessageType>
class Task: public BaseProcessor<ConfigType, DataMessageType> {
 public:
  typedef  BaseFunctor<ConfigType, DataMessageType> Functor;
  // 初始化
  bool Init(const TaskConfig& task_config) {
    for (auto functor_config : task_config.functor_config()) {
      // Functor* functor = CREATE_FUNCTOR(functor_config.name(), Functor);
      // BaseProcessor<ConfigType, DataMessageType>::AddFunctor(functor);
    }
    return true;
  }
  // 同步数据地址和配置
  void Sync(DataMessageType* data_message, ConfigType* config) {
    data_message_ = data_message_;
    config_ = config;
  }
  // 执行
  bool Run() {
    return DoProcess(data_message_);
  }

 private:
  // 处理器，之间是并行的关系
  std::vector<Functor*> functors_;
  // 消息
  DataMessageType* data_message_;
  // 业务配置
  ConfigType* config_;
};

}  // namespace common
}  // namespace gdt

#endif  // COMMON_STATE_MACHINE_TASK_H_
