// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17

#ifndef COMMON_SYSTEM_CONCURRENCY_THREADWORKER_H_
#define COMMON_SYSTEM_CONCURRENCY_THREADWORKER_H_

#include <string>

#include "common/base/callback.h"
#include "common/base/shared_ptr.h"
#include "thirdparty/glog/logging.h"

namespace gdt {

class ThreadWorker {
 public:
  ~ThreadWorker();
  static shared_ptr<ThreadWorker>
    Create(const std::string &name);
  bool PushTask(gdt::Closure *t);
  void Start();
  void Stop();
  void set_cpu(int32_t cpu);
  // Get the current thread executor;
  static shared_ptr<ThreadWorker> current_executor();
  struct Impl;
 private:
  explicit ThreadWorker(const std::string &name);
  Impl *impl_;
};

}  // namespace gdt

#endif  // COMMON_SYSTEM_CONCURRENCY_THREADWORKER_H_
