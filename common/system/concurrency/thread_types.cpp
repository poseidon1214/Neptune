// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/31/11

#include "common/system/concurrency/thread_types.h"
#include <string>
#include "common/system/check_error.h"

namespace gdt {

ThreadAttributes::ThreadAttributes() : affinity_mask_(0) {
  CHECK_PTHREAD_ERROR(pthread_attr_init(&attr_));
}

ThreadAttributes& ThreadAttributes::SetName(const std::string& name) {
  name_ = name;
  return *this;
}

ThreadAttributes::~ThreadAttributes() {
  CHECK_PTHREAD_ERROR(pthread_attr_destroy(&attr_));
}

ThreadAttributes& ThreadAttributes::SetStackSize(size_t size) {
  CHECK_PTHREAD_ERROR(pthread_attr_setstacksize(&attr_, size));
  return *this;
}

ThreadAttributes& ThreadAttributes::SetDetached(bool detached) {
  int state = detached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE;
  CHECK_PTHREAD_ERROR(pthread_attr_setdetachstate(&attr_, state));
  return *this;
}

ThreadAttributes& ThreadAttributes::SetPriority(int priority) {
  return *this;
}

ThreadAttributes& ThreadAttributes::SetAffinity(int cpu_index) {
  affinity_mask_ = 1ULL << cpu_index;
  return *this;
}

bool ThreadAttributes::IsDetached() const {
  int state = 0;
  CHECK_PTHREAD_ERROR(pthread_attr_getdetachstate(&attr_, &state));
  return state == PTHREAD_CREATE_DETACHED;
}

}  // namespace gdt
