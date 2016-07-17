// Copyright (c) 2014 Tencent Inc.
// Author: Li Meng (elvisli@tencent.com)
// Task means a module that process business logic, include :
// 1. build request, call rpc client module and process response
// 2. call other task and make them as a task flow

#ifndef COMMON_STATE_MACHINE_BASE_TASK_H_
#define COMMON_STATE_MACHINE_BASE_TASK_H_

#include <string>

#include "app/qzap/common/base/callback.h"

namespace gdt {
namespace common {

class BaseTask {
 public:
  enum TaskFinalStatusEnum {
    kSuccess = 0,
    kFailure = 1,
    kSkiped = 2,  // task do not run or do nothing actually
  };
  typedef int TaskFinalStatus;

  explicit BaseTask(const std::string& name);
  virtual ~BaseTask();

  // !!!Important, whenever done is called, the caller may delete the
  // the task, ALWAYS DO NOTHING after done->Run() is called.
  // There would be implicit call to the done->Run(), for example:
  //  {
  //    rpc->Call(..., done);
  //    this->xxx;
  //  }
  // the done->Run() will be called in another thread, and maybe (although
  // in some rare case) before this->xxx.
  void Start(Closure* done);

  const std::string& get_task_name() const { return task_name_; }

  // return total time cost when task finished, millisecond
  uint64_t GetTotalTimeCost();
  // return final status when task finished
  virtual TaskFinalStatus GetFinalStatus() { return task_final_status_; }

 protected:
  // Whether need to start the task. If return false, DoStart, BeforeFinish
  // and BeforeExit will not be called.
  virtual bool NeedStart();
  // When DoStart is called, unless SetFinalStatusAndExitTask is called
  // inside DoStart, the BeforeFinish() will be called.
  virtual void DoStart(Closure* done);
  // This is designed to be called after the async call.
  // TODO(yuxie): consider rename it to ProcessResponse.
  virtual void BeforeFinish();
  void SetFinalStatus(TaskFinalStatus task_final_status) {
    task_final_status_ = task_final_status;
  }

  void SetFinalStatusAndExitTask(TaskFinalStatus task_final_status) {
    task_final_status_ = task_final_status;
    error_exit_ = true;
  }

  // This is called right before the Closure in DoStart is called.
  // Put the business logic code in BeforeFinish(), we usually log or count
  // here.
  virtual void BeforeExit();

 private:
  void Finish(Closure* done);

  uint64_t start_time_;
  uint64_t finish_time_;
  TaskFinalStatus task_final_status_;
  std::string task_name_;
  bool error_exit_;
};

}  // namespace common
}  // namespace gdt

#endif  // COMMON_STATE_MACHINE_BASE_TASK_H_

