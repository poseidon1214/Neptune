// Copyright (c) 2015 Tencent Inc.
// Author: Xie Yu (yuxie@tencent.com)

#ifndef COMMON_STATE_MACHINE_STATE_MACHINE_H_
#define COMMON_STATE_MACHINE_STATE_MACHINE_H_

#include <map>
#include <utility>
#include <vector>

#include "app/qzap/common/base/base.h"
#include "common/base/callback.h"
#include "common/state_machine/base_task.h"

namespace gdt {
namespace common {

// StateMachine 是用来运行一些列BaseTask的
// 调用者需要为每个请求创建单独的StateMachine
// 一般的调用过程为：
//   StateMachine state_machine;
//   state_machine.AddState(1, NewPermanentCallback(xxxxx));
//   state_machine.AddState(2, NewPermanentCallback(xxxxxx));
//   state_machine.AddTransition(1, BaseTask::kSuccess, 2);
//   state_machine.AddTransition(2,
//                               BaseTask::kSuccess,
//                               StateMachine::kStateEnd);
//   state_machine.Start(1, NewCallback(xxxxx);
// 以上过程需在同一线程中进行，或者调用者自行加锁保证线程安全。

template <class DataMessageType>
class StateMachine {
 public:
  typedef Callback<BaseTask*()> TaskCreator;
  // kStateEnd是唯一预定义State，其他State由使用者定义，
  // 且!=kStateEnd
  static const int kStateEnd;

  StateMachine();
  virtual ~StateMachine();

  // 加入一个状态。当状态机运行到该状态时，
  // 状态机调用task_creator创建task并运行。
  //
  // 注意！！！task_creator必须是PermanentCallback。
  // StateMachine拥有task_creator，
  // 创建出的task也为StateMachine所有;
  // 如果创建出的task为NULL，则跳过该状态。
  virtual void AddState(int state, TaskCreator* task_creator);

  // 在current_state下，current_state对应的BaseTask的执行结果为
  // status时，转入next_state状态。
  // 当状态机运行至一个未定义的current_state及status时，
  // 状态机立即结束，且RanStatesAndResults的结果
  // 中不会出现kStateEnd.
  virtual void AddTransition(int current_state,
                             BaseTask::TaskFinalStatus task_result,
                             int next_state);
  // 加入默认跳转
  // 当没有AddTransition加入的跳转时，走这个。
  virtual void AddDefaultTransition(int current_state, int next_state);

  // 从start_state状态启动状态机
  // 所有的状态必须都能最终转到kStateEnd状态，
  // 当状态机进入kStateEnd状态，或者状态机无法再继续运转，
  // 调用done.
  // Start() 只可以调用一次
  // Start() 后不可以再调用AddState, AddTransition
  virtual void Start(int start_state, Closure* done);

  // 可以从这里拿到所以已经经历过的状态及运行结果
  // 调用者可以通过这个来判断状态机的运行结果。
  virtual const std::vector<std::pair<int, BaseTask::TaskFinalStatus> >&
      RanStatesAndResults() const;

  bool NextState(int current_state,
                 BaseTask::TaskFinalStatus task_result,
                 int* next_state);

 private:
  void RunCurrentState();
  void CurrentStateDone(BaseTask* task);
  bool SuccessCanReachEnd(int start_state);

  // Whether the adding transition states is valid.
  // This is used for sanity check for AddTransition and AddDefaultTransition.
  bool IsValidTransitionState(int current_state, int next_state) const;

  std::map<int, TaskCreator*> state_creators_;
  std::map<int, int> default_transitions_;
  std::map<std::pair<int, BaseTask::TaskFinalStatus>, int> transitions_;
  int current_state_;
  bool started_;
  Closure* final_done_;
  std::vector<std::pair<int, BaseTask::TaskFinalStatus> >
      ran_states_and_results_;

  DISALLOW_COPY_AND_ASSIGN(StateMachine);
};

}  // namespace common
}  // namespace gdt

#endif  // COMMON_STATE_MACHINE_STATE_MACHINE_H_
