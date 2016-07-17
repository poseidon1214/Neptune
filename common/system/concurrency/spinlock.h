// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2010-06-18

#ifndef COMMON_SYSTEM_CONCURRENCY_SPINLOCK_H
#define COMMON_SYSTEM_CONCURRENCY_SPINLOCK_H

#include <errno.h>
#include <stdlib.h>

#include <pthread.h>
#include "common/system/concurrency/scoped_locker.h"

namespace gdt {

/// spinlock is faster than mutex at some condition, but
class Spinlock {
 public:
  typedef ScopedLocker<Spinlock> Locker;
 public:
  Spinlock();
  ~Spinlock();
  void Lock();
  bool TryLock();
  void Unlock();
 private:
  Spinlock(const Spinlock&);
  Spinlock& operator=(const Spinlock&);
 private:
  pthread_spinlock_t m_lock;
  pid_t m_owner;
};

// compatible reason
typedef Spinlock::Locker SpinlockLocker;

} // namespace gdt

#endif // COMMON_SYSTEM_CONCURRENCY_SPINLOCK_H

