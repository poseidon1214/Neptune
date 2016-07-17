// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2011-08-31 01:29:15
// Description: thread local storage implementation

#include "common/system/concurrency/thread_local.h"
#include "common/system/check_error.h"

namespace gdt {
namespace details {

ThreadLocalSlot::ThreadLocalSlot(void (*destructor)(void* p)) {
  CHECK_PTHREAD_ERROR(pthread_key_create(&m_key, destructor));
}

ThreadLocalSlot::~ThreadLocalSlot() {
  CHECK_PTHREAD_ERROR(pthread_key_delete(m_key));
  m_key = pthread_key_t(); // reset to zero
}

void* ThreadLocalSlot::Get() const {
  return pthread_getspecific(m_key);
}

void ThreadLocalSlot::Set(void* p) {
  // if you meet error here, maybe this object has already been destructed
  CHECK_PTHREAD_ERROR(pthread_setspecific(m_key, p));
}

} // end namespace details
}
