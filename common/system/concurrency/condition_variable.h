// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_CONCURRENCY_CONDITION_VARIABLE_H_
#define COMMON_SYSTEM_CONCURRENCY_CONDITION_VARIABLE_H_

#include <assert.h>
#include <pthread.h>

#include "common/system/concurrency/mutex.h"

namespace gdt {

class ConditionVariable {
 public:
  ConditionVariable();
  ~ConditionVariable();
  void Signal();
  void Broadcast();

  // If timeout_in_ms is -1, it means infinite and equals to
  // Wait(Mutex* mutex);
  bool TimedWait(MutexBase* mutex, int timeout_in_ms);
  void Wait(MutexBase* mutex);
 private:
  void CheckValid() const;
 private:
  pthread_cond_t m_hCondition;
};

}  // namespace gdt

class CondVar : public gdt::ConditionVariable {
 public:
  void Notify() { Signal(); }
  void NotifyAll() { Broadcast(); }
};

#endif  // COMMON_SYSTEM_CONCURRENCY_CONDITION_VARIABLE_H_

