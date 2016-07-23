// Copyright (c) 2015 Tencent Inc.
// Author: Wang Qian (cernwang@tencent.com)

#ifndef COMMON_STATE_MACHINE_STATE_H_
#define COMMON_STATE_MACHINE_STATE_H_

#include <map>
#include <utility>
#include <vector>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <future>
#include "app/qzap/common/base/base.h"
#include "framework/state_machine/task.h"

namespace gdt {
namespace common {

#define CHECK(result)  \
  if (!(result)) { \
    return false; \
  }

typedef BitMap FinalStatus;

template <class ConfigType, class DataMessageType>
class State {
 public:
  typedef Task<ConfigType, DataMessageType> TaskType;
  // 初始化
  bool Init(const StateConfig& state_config) {
    for (auto task_config : state_config.task_config()) {
      TaskType task;
      task.Init(task_config);
      tasks.push_back(task);
    }
    timeout_ = state_config.timeout();
    return true;
  }
  // 同步数据地址和业务配置
  void Sync(DataMessageType* data_message, ConfigType* config) {
    data_message_ = data_message_;
    config_ = config;
  }
  // 执行
  FinalStatus Run() {
    std::vector<std::future<bool> > futures(tasks.size());
    for (size_t i = 0; i < tasks.size(); i++) {
      futures[i] = std::async(std::launch::async, 
                              std::bind(&TaskType::Run, &tasks[i])); 
    }
    do {
      LOG(INFO) << "Waiting ...";
    } while (Ready(futures));
    return status_;
  }

 private:
  // 是否准备好
  bool Ready(std::vector<std::future<bool> >& futures) {
    for (size_t i = 0; i < futures.size(); i++) {
      // 此处逻辑有问题 TODO（cernwang)
      CHECK(futures[i].wait_for(std::chrono::seconds(timeout_)) == std::future_status::ready);
      SetStatus(futures[i].get(), i, &status_);
    }
    return true;
  }

  void SetStatus(bool result, uint64_t index, FinalStatus* status) {
    // TODO
    return;
  }

 private:
  // 任务，之间是并行的关系
  std::vector<TaskType> tasks;
  // 超时
  uint64_t timeout_;
  // 状态
  FinalStatus status_;
  // 消息
  DataMessageType* data_message_;
  // 业务配置
  ConfigType* config_;
};

}  // namespace common
}  // namespace gdt

#endif  // COMMON_STATE_MACHINE_STATE_H_
