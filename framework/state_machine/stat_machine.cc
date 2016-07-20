// Copyright (c) 2015 Tencent Inc.
// Author: Xie Yu (yuxie@tencent.com)

#include "common/state_machine/state_machine.h"

#include <list>
#include <map>
#include <set>
#include <utility>

#include "common/base/callback.h"
#include "common/state_machine/base_task.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_bool(gdt_state_machine_check_success_route, true,
            "Check whether we can reach kStateEnd without loop from start."
            " Currently the business logic is that we can always reach"
            " kStateEnd by going the kSuccess route. But this maybe not true"
            " in the future. So we have a flag for this check."
            " Also, turn off this flag may save a tiny little cpu time.");

namespace gdt {
namespace common {

namespace {
// Copied from "thirdparty/protobuf-3.0.0-alpha-2/src/google/protobuf/stubs/stl_util.h"
// Not include it to avoid adding dependency for the whole google protobuf.
// Consider to extract it out, then we could include it without addind
// dependency to google protobuf.
//
// Given an STL container consisting of (key, value) pairs, STLDeleteValues
// deletes all the "value" components and clears the container.  Does nothing
// in the case it's given a NULL pointer.
template <class T>
void STLDeleteValues(T *v) {
  if (!v) return;
  for (typename T::iterator i = v->begin(); i != v->end(); ++i) {
    delete i->second;
  }
  v->clear();
}
}  // namespace

const int StateMachine::kStateEnd = 0;

StateMachine::StateMachine()
  : current_state_(kStateEnd),
    started_(false),
    final_done_(NULL) {
}

StateMachine::~StateMachine() {
  STLDeleteValues(&state_creators_);
}

void StateMachine::AddState(
    int state, TaskCreator* task_creator) {
  CHECK(!started_);
  CHECK_NE(state, kStateEnd)
      << "kStateEnd = 0 is reserved, never add it.";
  CHECK_NOTNULL(task_creator);
  CHECK(task_creator->IsRepeatable())
      << "The callback is not PermanentCallback for: " << state;
  state_creators_.insert(std::make_pair(state, task_creator));
}

void StateMachine::AddTransition(
    int current_state, BaseTask::TaskFinalStatus task_result, int next_state) {
  CHECK(IsValidTransitionState(current_state, next_state));
  transitions_.insert(
      std::make_pair(std::make_pair(current_state, task_result), next_state));
}

void StateMachine::AddDefaultTransition(
    int current_state, int next_state) {
  CHECK(IsValidTransitionState(current_state, next_state));
  default_transitions_.insert(std::make_pair(current_state, next_state));
}

void StateMachine::Start(int start_state, Closure* done) {
  CHECK(!started_);
  started_ = true;
  CHECK_NOTNULL(done);
  CHECK(state_creators_.find(start_state) != state_creators_.end())
      << "Invalid start state.";
  CHECK(SuccessCanReachEnd(start_state))
      << "Could not reach kStateEnd through kSuccess route.";
  current_state_ = start_state;
  final_done_ = done;
  RunCurrentState();
}

const std::vector<std::pair<int, BaseTask::TaskFinalStatus> >&
StateMachine::RanStatesAndResults() const {
  return ran_states_and_results_;
}

bool StateMachine::SuccessCanReachEnd(int start_state) {
  if (!FLAGS_gdt_state_machine_check_success_route) {
    return true;
  }
  std::set<int> ran_state;
  int state = start_state;
  while (state != kStateEnd) {
    ran_state.insert(state);
    int next_state;
    if (!NextState(state, BaseTask::kSuccess, &next_state)) {
      LOG(ERROR) << "Could not reach kStateEnd, no route going forward"
                 << " for state: " << state << " in result kSuccess.";
      return false;
    }
    if (ran_state.find(next_state) != ran_state.end()) {
      LOG(ERROR) << "Loop found in the success route to kStateEnd.";
      return false;
    }
    state = next_state;
  }
  return true;
}

void StateMachine::RunCurrentState() {
  VLOG(500) << "Running state: " << current_state_;
  if (current_state_ == kStateEnd) {
    VLOG(100) << "Reach kStateEnd";
    ran_states_and_results_.push_back(std::make_pair(kStateEnd, 0));
    final_done_->Run();
    return;
  }
  std::map<int, TaskCreator*>::iterator it =
      state_creators_.find(current_state_);
  if (it == state_creators_.end()) {
    LOG(ERROR) << "State not found, this should not happen.";
    final_done_->Run();
    return;
  }
  BaseTask* task = it->second->Run();
  if (task == NULL) {
    VLOG(500) << "NULL task created for state: " << current_state_
              << " will ignore, and return kSkiped result.";
    CurrentStateDone(NULL);
    return;
  }
  Closure* done = NewCallback(this, &StateMachine::CurrentStateDone, task);
  task->Start(done);
  // CurrentStateDone() may already run, nothing should be done in this
  // function after this line ----------------------------------------.
}

void StateMachine::CurrentStateDone(BaseTask* task) {
  // If task is NULL, default result as kSkiped.
  BaseTask::TaskFinalStatus task_result = BaseTask::kSkiped;
  if (task != NULL) {
    task_result = task->GetFinalStatus();
    delete task;
  }
  VLOG(500) << "State done: " << current_state_
            << " with task_result: " << task_result;
  ran_states_and_results_.push_back(
      std::make_pair(current_state_, task_result));
  int next_state;
  bool next_state_found = NextState(current_state_, task_result, &next_state);
  if (next_state_found) {
    current_state_ = next_state;
    RunCurrentState();
  } else {
    LOG(ERROR) << "Could not find next state in current_state: "
               << current_state_ << " and task result: " << task_result
               << " will exit the state machine.";
    final_done_->Run();
  }
}

bool StateMachine::NextState(
    int current_state, BaseTask::TaskFinalStatus task_result, int* next_state) {
  std::map<std::pair<int, BaseTask::TaskFinalStatus>, int>::iterator it =
    transitions_.find(std::make_pair(current_state, task_result));
  if (it != transitions_.end()) {
    *next_state = it->second;
    return true;
  } else {
    std::map<int, int>::iterator default_it =
      default_transitions_.find(current_state);
    if (default_it != default_transitions_.end()) {
      *next_state = default_it->second;
      return true;
    }
  }
  return false;
}

bool StateMachine::IsValidTransitionState(
    int current_state, int next_state) const {
  if (started_) {
    LOG(ERROR) << "Never call AddTransition after Start()";
    return false;
  }
  if (state_creators_.find(current_state) == state_creators_.end()) {
    LOG(ERROR) << "Invalid current_state: " << current_state;
    return false;
  }
  if (next_state != kStateEnd &&
      state_creators_.find(next_state) == state_creators_.end()) {
    LOG(ERROR) << "Invalid next_state: " << next_state;
    return false;
  }
  return true;
}

}  // namespace common
}  // namespace gdt
