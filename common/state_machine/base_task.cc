// Copyright (c) 2014 Tencent Inc.
// Author: Li Meng (elvisli@tencent.com)

#include "common/state_machine/base_task.h"

#include "common/system/time/gettimestamp.h"
#include "thirdparty/glog/logging.h"

namespace gdt {
namespace common {

BaseTask::BaseTask(const std::string& name)
    : start_time_(0),
      finish_time_(0),
      task_final_status_(kSuccess),
      task_name_(name),
      error_exit_(false) {
}

BaseTask::~BaseTask() {
}

bool BaseTask::NeedStart() {
  return true;
}

void BaseTask::Start(Closure* done) {
  if (!NeedStart()) {
    VLOG(10) << "Not need start " << get_task_name();
    SetFinalStatus(kSkiped);
    done->Run();
    return;
  }
  start_time_ = GetTimeStampInMs();
  VLOG(10) << "Start " << get_task_name();
  Closure* finish = ::NewCallback(this, &BaseTask::Finish, done);
  DoStart(finish);
}

uint64_t BaseTask::GetTotalTimeCost() {
  if (start_time_ == 0)  // call function before task start
    return 0;
  if (finish_time_ == 0)  // call function before task finished
    finish_time_ = GetTimeStampInMs();
  return finish_time_ <= start_time_ ? 0 : finish_time_ - start_time_;
}

void BaseTask::Finish(Closure* done) {
  // TODO(yuxie): The done passed in is always the done passed to Start(),
  // consider set the "done" as a member variable.
  if (!error_exit_) {
    BeforeFinish();
  } else {
    VLOG(10) << "Error exit " << get_task_name();
  }
  VLOG(10) << "Finish " << get_task_name();
  finish_time_ = GetTimeStampInMs();
  int time_cost = static_cast<int>(GetTotalTimeCost());
  VLOG(10) << "Time cost of " << get_task_name() << ": " << time_cost;

  BeforeExit();
  done->Run();
}

void BaseTask::DoStart(Closure* done) {
  done->Run();
}

void BaseTask::BeforeFinish() {
}

void BaseTask::BeforeExit() {
}

}  // namespace common
}  // namespace gdt

