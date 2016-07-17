// Copyright (c) 2012, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2012-04-13
// Description:

#include "common/system/concurrency/spinlock.h"
#include "common/system/check_error.h"
#include "common/system/concurrency/this_thread.h"

namespace gdt {

Spinlock::Spinlock() {
  CHECK_PTHREAD_ERROR(pthread_spin_init(&m_lock, 0));
  m_owner = 0;
}

Spinlock::~Spinlock() {
  CHECK_PTHREAD_ERROR(pthread_spin_destroy(&m_lock));
  m_owner = -1;
}

void Spinlock::Lock() {
  CHECK_PTHREAD_ERROR(pthread_spin_lock(&m_lock));
  m_owner = ThisThread::GetId();
}

bool Spinlock::TryLock() {
  if (CHECK_PTHREAD_TRYLOCK_ERROR(pthread_spin_trylock(&m_lock))) {
    m_owner = ThisThread::GetId();
    return true;
  }
  return false;
}

void Spinlock::Unlock() {
  m_owner = 0;
  CHECK_PTHREAD_ERROR(pthread_spin_unlock(&m_lock));
}

}
