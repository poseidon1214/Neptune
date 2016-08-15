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
#include "framework/common/bitmap.h"
#include "framework/state_machine/task.h"

namespace gdt {
namespace common {

typedef BitMap FinalStatus;

template <class ConfigType, class DataMessageType>
class State {
 public:
  typedef Task<ConfigType, DataMessageType> TaskType;
  // 初始化
  bool Init(const StateConfig& state_config) {
    for (auto task_config : state_config.task_config()) {
      TaskType task;
      task.Sync(data_message_, config_);
      task.Init(task_config);
      tasks.push_back(task);
    }
    std::copy(state_config.transition_config().begin(),
              state_config.transition_config().end(),
              std::back_inserter(transition_config_));
    default_next_state_id_ = state_config.default_next_state_id();
    timeout_ = state_config.timeout();
    return true;
  }
  // 同步数据地址和业务配置
  void Sync(DataMessageType* data_message, ConfigType* config) {
    data_message_ = data_message;
    config_ = config;
  }
  // 执行
  FinalStatus Run() {
    std::vector<std::future<bool> > futures(tasks.size());
    for (size_t i = 0; i < tasks.size(); i++) {
      // 任务之间异步并行
      futures[i] = std::async(std::launch::async, 
                              std::bind(&TaskType::Run, &tasks[i])); 
    }
    do {
      LOG(ERROR) << "Waiting ...";
    } while (!Ready(futures));
    return status_;
  }
  // 获取下一个状态 TODO(cernwang)
  int64_t NextState(const FinalStatus& status) {
    // 按照顺序第一个符合的直接跳转
    auto iter = std::find_if(transition_config_.begin(), transition_config_.end(), 
                             [=](const TransitionConfig& config)->bool {
                               return Include(status, config.status());
                             });
    return iter == transition_config_.end() ? default_next_state_id_ : iter->next_state_id();
  }

 private:
  // 是否准备好
  bool Ready(std::vector<std::future<bool> >& futures) {
    for (size_t i = 0; i < futures.size(); i++) {
      // 此处逻辑有问题 TODO（cernwang)
      CHECK(futures[i].wait_for(std::chrono::seconds(timeout_)) == std::future_status::ready);
      // 成功则设置状态
      SetBit(tasks[i].Id(), futures[i].get(), &status_);
    }
    return true;
  }

 private:
  // 任务，之间是并行的关系
  std::vector<TaskType> tasks;
  // 超时
  uint64_t timeout_;
  // 状态
  FinalStatus status_;
  // 变换配置
  std::vector<TransitionConfig> transition_config_;
  // 消息
  DataMessageType* data_message_;
  // 业务配置
  ConfigType* config_;
  // 默认跳转ID
  int64_t default_next_state_id_;
};

}  // namespace common
}  // namespace gdt

#endif  // COMMON_STATE_MACHINE_STATE_H_
