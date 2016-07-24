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
#include "common/base/class_register.h"
#include "data_collector/feeder/framework/data_message.h"
#include "framework/common/base_processor.h"
#include "framework/proto/framework.pb.h"

namespace gdt {
namespace common {

typedef BitMap FinalStatus;

template <class ConfigType, class DataMessageType>
class Task: public BaseProcessor<ConfigType, DataMessageType> {
 public:
  typedef  BaseFunctor<ConfigType, DataMessageType> Functor;

  // 初始化
  bool Init(const TaskConfig& task_config) {
    name_ = task_config.name();
    id_ = task_config.task_id();
    for (auto functor_config : task_config.functor_config()) {
      // 注意register的include顺序在本文件之前，否则此处将发生编译错误
      // 这边是一个不合理的地方，但是暂时没有想到好的办法处理
      Functor* functor = CREATE_FUNCTOR(functor_config.name(), Functor);
      LOG(ERROR) << functor_config.name();
      LOG(ERROR) << functor;
      CHECK(AddFunctor(functor));
    }
    this->success_init_ = true;
    return true;
  }
  // 同步数据地址和配置
  void Sync(DataMessageType* data_message, ConfigType* config) {
    data_message_ = data_message;
    this->config_ = config;
  }
  // 执行
  bool Run() {
    LOG(ERROR) << "Runing Task" << name_;
    return DoProcess(data_message_);
  }
  // ID
  uint64_t Id() {
    return id_;
  }
 public:
  // 消息
  DataMessageType* data_message_;
  // 名称
  std::string name_;
  // ID
  uint64_t id_;
};

}  // namespace common
}  // namespace gdt

#endif  // COMMON_STATE_MACHINE_TASK_H_
