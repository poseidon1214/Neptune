// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_CONCURRENCY_RWLOCK_H
#define COMMON_SYSTEM_CONCURRENCY_RWLOCK_H

/// @file
/// @author phongche <phongchen@tencent.com>
/// @date Nov 28, 2010

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <string>

#include "common/system/check_error.h"
#include "common/system/concurrency/scoped_locker.h"

#if defined __unix__

#include <pthread.h>

namespace gdt {

/// Reader/Writer lock
class RWLock {
 public:
  typedef ScopedReaderLocker<RWLock> ReaderLocker;
  typedef ScopedWriterLocker<RWLock> WriterLocker;
  typedef ScopedTryReaderLocker<RWLock> TryReaderLocker;
  typedef ScopedTryWriterLocker<RWLock> TryWriterLocker;

  enum Kind {
    kKindPreferReader = PTHREAD_RWLOCK_PREFER_READER_NP,

    // Setting the value read-write lock kind to PTHREAD_RWLOCK_PREFER_WRITER_NP,
    // results in the same behavior as setting the value to PTHREAD_RWLOCK_PREFER_READER_NP.
    // As long as a reader thread holds the lock the thread holding a write lock will be
    // starved. Setting the kind value to PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
    // allows the writer to run. However, the writer may not be recursive as is implied by the
    // name.
    kKindPreferWriter = PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
    kKindDefault = PTHREAD_RWLOCK_DEFAULT_NP,
  };

 public:
  RWLock() {
    // note: default rwlock is prefer reader
    CHECK_PTHREAD_ERROR(pthread_rwlock_init(&m_lock, NULL));
  }

  explicit RWLock(Kind kind) {
    pthread_rwlockattr_t attr;
    CHECK_PTHREAD_ERROR(pthread_rwlockattr_init(&attr));
    CHECK_PTHREAD_ERROR(pthread_rwlockattr_setkind_np(&attr, kind));
    CHECK_PTHREAD_ERROR(pthread_rwlock_init(&m_lock, &attr));
    CHECK_PTHREAD_ERROR(pthread_rwlockattr_destroy(&attr));
  }

  ~RWLock() {
    CHECK_PTHREAD_ERROR(pthread_rwlock_destroy(&m_lock));
    memset(&m_lock, 0xFF, sizeof(m_lock));
  }

  void ReaderLock() {
    CheckValid();
    CHECK_PTHREAD_ERROR(pthread_rwlock_rdlock(&m_lock));
  }

  bool TryReaderLock() {
    CheckValid();
    return CHECK_PTHREAD_TRYLOCK_ERROR(pthread_rwlock_tryrdlock(&m_lock));
  }

  void WriterLock() {
    CheckValid();
    CHECK_PTHREAD_ERROR(pthread_rwlock_wrlock(&m_lock));
  }

  bool TryWriterLock() {
    CheckValid();
    return CHECK_PTHREAD_TRYLOCK_ERROR(pthread_rwlock_trywrlock(&m_lock));
  }

  void Unlock() {
    CheckValid();
    CHECK_PTHREAD_ERROR(pthread_rwlock_unlock(&m_lock));
  }

  // names for scoped lockers
  void ReaderUnlock() {
    Unlock();
  }

  void WriterUnlock() {
    Unlock();
  }

 public: // only for test and debug, should not be used in normal code logical
  bool IsReaderLocked() const {
    // accessing pthread private data: nonportable but no other way
    return m_lock.__data.__nr_readers != 0;
  }

  bool IsWriterLocked() const {
    return m_lock.__data.__writer != 0;
  }

  bool IsLocked() const {
    return IsReaderLocked() || IsWriterLocked();
  }

 private:
  void CheckValid() const {
#ifdef __GLIBC__
    // If your program crashed here at runtime, maybe the rwlock object
    // has been destructed.
    if (memcmp(&m_lock.__data.__flags, "\xFF\xFF\xFF\xFF", 4) == 0)
      CHECK_ERRNO_ERROR(EINVAL);
#endif
  }

 private: // forbid copying
  RWLock(const RWLock& src);
  RWLock& operator=(const RWLock& rhs);

 private:
  pthread_rwlock_t m_lock;
};

}  // namespace gdt

#elif defined _WIN32

#include "common/system/concurrency/mutex.h"

namespace gdt {

// fallback to mutex
class RWLock {
 public:
  typedef ScopedReaderLocker<RWLock> ReaderLocker;
  typedef ScopedWriterLocker<RWLock> WriterLocker;
  typedef ScopedTryReaderLocker<RWLock> TryReaderLocker;
  typedef ScopedTryWriterLocker<RWLock> TryWriterLocker;

 public:
  RWLock() : m_lock_count(0) {
  }

  void ReaderLock() {
    m_mutex.Lock();
    ++m_lock_count;
  }

  bool TryReaderLock() {
    bool ret = m_mutex.TryLock();
    if (ret)
      ++m_lock_count;
    return ret;
  }

  void WriterLock() {
    m_mutex.Lock();
    --m_lock_count;
  }

  bool TryWriterLock() {
    bool ret = m_mutex.TryLock();
    if (ret)
      --m_lock_count;
    return ret;
  }

  void Unlock() {
    m_lock_count = 0;
    m_mutex.Unlock();
  }

  void ReaderUnlock() {
    Unlock();
  }

  void WriterUnlock() {
    Unlock();
  }

 public: // only for test and debug, should not be used in normal code logical
  bool IsReaderLocked() const {
    return m_lock_count > 0;
  }

  bool IsWriterLocked() const {
    return m_lock_count < 0;
  }

  bool IsLocked() const {
    return m_lock_count != 0;
  }

 private:
  // mutex is noncopyable, so copy ctor and assign can't be generated
  Mutex m_mutex;

  // > 0: reader locked
  // < 0: writer locker
  // = 0: unlocked
  int m_lock_count;
};

}  // namespace gdt

#endif

#endif // COMMON_SYSTEM_CONCURRENCY_RWLOCK_H