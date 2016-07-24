// Copyright (c) 2015 Tencent Inc.
// Author: Wang Qian (cernwang@tencent.com)

#ifndef COMMON_STATE_MACHINE_STATE_MACHINE_H_
#define COMMON_STATE_MACHINE_STATE_MACHINE_H_

#include <map>
#include <utility>
#include <vector>


#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/base.h"
#include "framework/proto/framework.pb.h"
#include "framework/state_machine/state.h"

namespace gdt {
namespace common {

typedef BitMap FinalStatus;



template <class ConfigType, class DataMessageType>
class StateMachine {
 public:
  typedef State<ConfigType, DataMessageType> StateType;
  // 初始化
  bool Init(const StateMachineConfig& state_machine_config) {
    for (auto state_config : state_machine_config.state_config()) {
      StateType state;
      state.Sync(data_message_, config_);
      state.Init(state_config);
      states_[state_config.state_id()] = state;
    }
    current_state_ = state_machine_config.start_state_id();
    return true;
  }
  // 执行
  bool Run() {
    while (current_state_ > kEndStateId) {
      auto iter = states_.find(current_state_);
      CHECK_LOG(iter != states_.end(), current_state_);
      FinalStatus status = iter->second.Run();
      history_.push_back(std::make_pair(current_state_, status));
      current_state_ = iter->second.NextState(status);
    }
    // 等于0为正常退出
    return current_state_ == kEndStateId;
  }
  // 获取现有状态
  uint64_t GetCurrentState() const {
    return current_state_;
  }
  // 同步数据地址和业务配置
  void Sync(DataMessageType* data_message, ConfigType* config) {
    data_message_ = data_message;
    config_ = config;
  }

 private:
  // 状态ID到状态机
  std::map<uint64_t, StateType> states_;
  // 状态历史记录
  std::vector<std::pair<uint64_t, FinalStatus> > history_;
  // 现在的状态
  int64_t current_state_;
  // 最终状态
  const static uint64_t kEndStateId = 0;
  // 消息
  DataMessageType* data_message_;
  // 业务配置
  ConfigType* config_;
};

}  // namespace common
}  // namespace gdt

#endif  // COMMON_STATE_MACHINE_STATE_MACHINE_H_
