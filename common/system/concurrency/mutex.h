// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_CONCURRENCY_MUTEX_H
#define COMMON_SYSTEM_CONCURRENCY_MUTEX_H

#include <assert.h>
#include <errno.h>

# include <pthread.h>

#include <string.h>
#include <stdexcept>
#include <string>

#include "common/base/static_assert.h"
#include "common/system/check_error.h"
#include "common/system/concurrency/scoped_locker.h"

// GLOBAL_NOLINT(runtime/references)

namespace gdt {

class ConditionVariable;

class MutexBase {
 protected:
  // type converter, force enable errorcheck in debug mode
  static int DebugEnabled(int type) {
    if (type == PTHREAD_MUTEX_RECURSIVE)
      return type;
#ifdef NDEBUG
    return type;
#else
    // alwasy enable errcheck in debug mode
    return PTHREAD_MUTEX_ERRORCHECK_NP;
#endif
  }

  explicit MutexBase(int type) {
    pthread_mutexattr_t attr;
    CHECK_PTHREAD_ERROR(pthread_mutexattr_init(&attr));
    CHECK_PTHREAD_ERROR(pthread_mutexattr_settype(&attr, type));
    CHECK_PTHREAD_ERROR(pthread_mutex_init(&m_mutex, &attr));
    CHECK_PTHREAD_ERROR(pthread_mutexattr_destroy(&attr));
  }
  ~MutexBase() {
    CHECK_PTHREAD_ERROR(pthread_mutex_destroy(&m_mutex));
    // Since pthread_mutex_destroy will set __data.__kind to -1 and check
    // it in pthread_mutex_lock/pthread_mutex_unlock, nothing is necessary
    // to do for destructed access check.
  }
 public:
  void Lock() {
    CHECK_PTHREAD_ERROR(pthread_mutex_lock(&m_mutex));
    assert(IsLocked());
  }

  bool TryLock() {
    return CHECK_PTHREAD_TRYLOCK_ERROR(pthread_mutex_trylock(&m_mutex));
  }

  // for test and debug only
  bool IsLocked() const {
    // by inspect internal data
    return m_mutex.__data.__lock > 0;
  }

  void Unlock() {
    assert(IsLocked());
    CHECK_PTHREAD_ERROR(pthread_mutex_unlock(&m_mutex));
    // NOTE: can't check unlocked here, maybe already locked by other thread
  }
 private:
  MutexBase(const MutexBase& right);
  MutexBase& operator = (const MutexBase& right);
 private:
  pthread_mutex_t m_mutex;
  friend class ConditionVariable;
};

/// simple mutex, fast but nonrecursive
/// if same thread try to acquire the lock twice, deadlock would occur.
class SimpleMutex : public MutexBase {
 public:
  typedef ScopedLocker<SimpleMutex> Locker;
  SimpleMutex() : MutexBase(DebugEnabled(PTHREAD_MUTEX_DEFAULT)) {
  }
};

/// RecursiveMutex can be acquired by same thread multiple times, but slower than
/// SimpleMutex
class RecursiveMutex : public MutexBase {
 public:
  typedef ScopedLocker<RecursiveMutex> Locker;
  RecursiveMutex() : MutexBase(PTHREAD_MUTEX_RECURSIVE) {
  }
};

/// try to spin some time if can't acquire lock, if still can't acquire, wait.
class AdaptiveMutex : public MutexBase {
 public:
  typedef ScopedLocker<AdaptiveMutex> Locker;
  AdaptiveMutex() : MutexBase(DebugEnabled(PTHREAD_MUTEX_ADAPTIVE_NP)) {
  }
};

class Mutex : public MutexBase {
 public:
  typedef ScopedLocker<MutexBase> Locker;
  Mutex() : MutexBase(PTHREAD_MUTEX_ERRORCHECK_NP) {
  }
  template <typename T>
  explicit Mutex(const T& src) : MutexBase(PTHREAD_MUTEX_ERRORCHECK_NP) {
    STATIC_ASSERT(sizeof(T) < 0,
                  "Mutex is always non-recursive now. if you need recursive, please use RecursiveMutex");
  }
};

typedef ScopedLocker<MutexBase> MutexLocker;

// check ing missing variable name, eg MutexLocker(m_lock);
#define MutexLocker(x) STATIC_ASSERT(false, "Mising variable name of MutexLocker")

/// null mutex for template mutex param placeholder
/// NOTE: don't make this class uncopyable
class NullMutex {
 public:
  typedef ScopedLocker<NullMutex> Locker;
 public:
  NullMutex() : m_locked(false) {
  }

  void Lock() {
    m_locked = true;
  }

  bool TryLock() {
    m_locked = true;
    return true;
  }

  // by inspect internal data
  bool IsLocked() const {
    return m_locked;
  }

  void Unlock() {
    m_locked = false;
  }
 private:
  bool m_locked;
};

} // namespace gdt

#endif // COMMON_SYSTEM_CONCURRENCY_MUTEX_H

