// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17
// SharedLockable concept is a that allows for shared ownership as well as
// exclusive ownership. This is the standard multiple-reader / single-write
// model: at most one thread can have exclusive ownership, and if any thread
// does have exclusive ownership, no other threads can have shared or exclusive
// ownership. Alternatively, many threads may have shared ownership.

// The UpgradeLockable concept is a refinement of the SharedLockable concept
// that allows for upgradable ownership as well as shared ownership and
// exclusive ownership. This is an extension to the multiple-reader /
// single-write model provided by the SharedLockable concept: a single thread
// may have upgradable ownership at the same time as others have shared
// ownership. The thread with upgradable ownership may at any time attempt to
// upgrade that ownership to exclusive ownership. If no other threads have
// shared ownership, the upgrade is completed immediately, and the thread now
// has exclusive ownership, which must be relinquished by a call to unlock(),
// just as if it had been acquired by a call to lock().
// If a thread with upgradable ownership tries to upgrade whilst other threads
// have shared ownership, the attempt will fail and the thread will block until
// exclusive ownership can be acquired.

// Ownership can also be downgraded as well as upgraded: exclusive ownership of an
// implementation of the UpgradeLockable concept can be downgraded to upgradable
// ownership or shared ownership, and upgradable ownership can be downgraded to
// plain shared ownership.

#ifndef SHARED_MUTEX_H_
#define SHARED_MUTEX_H_
class SharedMutex {
 public:
  SharedMutex();
  ~SharedMutex();
  void LockShared();
  bool TrylockShared();
  bool TimedLockShared(int time_ms);
  void UnlockShared();
  void Lock();
  bool TimedLock(int time_ms);
  bool Trylock();
  void Unlock();
  void LockUpgrade();
  bool TimedLockUpgrade(int time_ms);
  bool TrylockUpgrade();
  void UnlockUpgrade();
  void UnlockUpgradeAndLock();
  void UnlockAndLockUpgrade();
  void UnlockAndLockShared();
  void UnlockUpgradeAndLockShared();
  struct Impl;
  class ScopedLock;
  class ScopedSharedLock;
  class ScopedUpgradeLock;
 private:
  Impl *impl_;
};

class SharedMutex::ScopedLock {
 public:
  explicit ScopedLock(SharedMutex *mu)
    : mu_(mu) {
      mu_->Lock();
  }
  ~ScopedLock() {
    mu_->Unlock();
  }
 private:
  SharedMutex *const mu_;
};

class SharedMutex::ScopedSharedLock {
 public:
  explicit ScopedSharedLock(SharedMutex *mu)
    : mu_(mu) {
      mu_->LockShared();
  }
  ~ScopedSharedLock() {
    mu_->UnlockShared();
  }
 private:
  SharedMutex *const mu_;
};

class SharedMutex::ScopedUpgradeLock {
 public:
  explicit ScopedUpgradeLock(SharedMutex *mu)
    : mu_(mu) {
      mu_->LockUpgrade();
  }
  ~ScopedUpgradeLock() {
    mu_->UnlockUpgrade();
  }
 private:
  SharedMutex *const mu_;
};
#endif  // SHARED_MUTEX_H_
