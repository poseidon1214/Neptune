// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/system/concurrency/condition_variable.h"

#include <assert.h>
#include <sys/time.h>

#include <string>

#include "common/system/check_error.h"
#include "common/system/time/posix_time.h"

namespace gdt {

ConditionVariable::ConditionVariable() {
  CHECK_PTHREAD_ERROR(pthread_cond_init(&m_hCondition, NULL));
}

ConditionVariable::~ConditionVariable() {
  CHECK_PTHREAD_ERROR(pthread_cond_destroy(&m_hCondition));
}

void ConditionVariable::CheckValid() const {
  // __total_seq will be set to -1 by pthread_cond_destroy
  assert(m_hCondition.__data.__total_seq != -1ULL &&
         "this cond has been destructed");
}

void ConditionVariable::Signal() {
  CheckValid();
  CHECK_PTHREAD_ERROR(pthread_cond_signal(&m_hCondition));
}

void ConditionVariable::Broadcast() {
  CheckValid();
  CHECK_PTHREAD_ERROR(pthread_cond_broadcast(&m_hCondition));
}

void ConditionVariable::Wait(MutexBase* mutex) {
  CheckValid();
  CHECK_PTHREAD_ERROR(pthread_cond_wait(&m_hCondition, &mutex->m_mutex));
}

bool ConditionVariable::TimedWait(MutexBase* mutex, int timeout_in_ms) {
  timespec ts;
  RelativeTimeInMillSecondsToAbsTimeInTimeSpec(timeout_in_ms, &ts);
  return CHECK_PTHREAD_TIMED_ERROR(pthread_cond_timedwait(&m_hCondition,
                                   &mutex->m_mutex, &ts));
}

}  // namespace gdt


