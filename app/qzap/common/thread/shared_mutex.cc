// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17
#include "app/qzap/common/thread/mutex.h"
#include "app/qzap/common/thread/shared_mutex.h"

struct SharedMutex::Impl {
 private:
  struct state_data {
    unsigned shared_count;
    bool exclusive;
    bool upgrade;
    bool exclusive_waiting_blocked;
  };
  state_data state;
  Mutex state_change;
  CondVar shared_cond;
  CondVar exclusive_cond;
  CondVar upgrade_cond;

  void ReleaseWaiters() {
    exclusive_cond.Notify();
    shared_cond.NotifyAll();
  }

 public:
  Impl() {
    state_data local_state = {0, 0, 0, 0};
    state = local_state;
  }

  ~Impl() {
  }

  void LockShared() {
    MutexLock lk(&state_change);

    while (state.exclusive ||
          state.exclusive_waiting_blocked) {
      shared_cond.Wait(&state_change);
    }
    ++state.shared_count;
  }

  bool TrylockShared() {
    MutexLock lk(&state_change);
    if (state.exclusive ||
       state.exclusive_waiting_blocked) {
      return false;
    } else {
      ++state.shared_count;
      return true;
    }
  }

  bool TimedLockShared(int timeout) {
    MutexLock lk(&state_change);

    while (state.exclusive ||
          state.exclusive_waiting_blocked) {
      if (!shared_cond.TimedWait(
          &state_change, timeout)) {
        return false;
      }
    }
    ++state.shared_count;
    return true;
  }

  void UnlockShared() {
    MutexLock lk(&state_change);
    bool const last_reader=!--state.shared_count;

    if (last_reader) {
      if (state.upgrade) {
        state.upgrade = false;
        state.exclusive = true;
        upgrade_cond.Notify();
      } else {
        state.exclusive_waiting_blocked = false;
      }
      ReleaseWaiters();
    }
  }

  void Lock() {
    MutexLock lk(&state_change);

    while (state.shared_count || state.exclusive) {
      state.exclusive_waiting_blocked = true;
      exclusive_cond.Wait(&state_change);
    }
    state.exclusive = true;
  }

  bool TimedLock(int timeout) {
    MutexLock lk(&state_change);
    while (state.shared_count ||
          state.exclusive) {
      state.exclusive_waiting_blocked = true;
      if (!exclusive_cond.TimedWait(&state_change, timeout)) {
        if (state.shared_count || state.exclusive) {
          state.exclusive_waiting_blocked = false;
          exclusive_cond.Notify();
          return false;
        }
        break;
      }
    }
    state.exclusive = true;
    return true;
  }

  bool Trylock() {
    MutexLock lk(&state_change);
    if (state.shared_count || state.exclusive) {
      return false;
    } else {
      state.exclusive = true;
      return true;
    }
  }

  void Unlock() {
    MutexLock lk(&state_change);
    state.exclusive = false;
    state.exclusive_waiting_blocked = false;
    ReleaseWaiters();
  }

  void LockUpgrade() {
    MutexLock lk(&state_change);
    while (state.exclusive ||
          state.exclusive_waiting_blocked ||
          state.upgrade) {
      shared_cond.Wait(&state_change);
    }
    ++state.shared_count;
    state.upgrade = true;
  }

  bool TimedLockUpgrade(int timeout) {
    MutexLock lk(&state_change);
    while (state.exclusive ||
          state.exclusive_waiting_blocked ||
          state.upgrade) {
      if (!shared_cond.TimedWait(&state_change, timeout)) {
        if (state.exclusive ||
           state.exclusive_waiting_blocked ||
           state.upgrade) {
          return false;
        }
        break;
      }
    }
    ++state.shared_count;
    state.upgrade = true;
    return true;
  }

  bool TrylockUpgrade() {
    MutexLock lk(&state_change);
    if (state.exclusive ||
       state.exclusive_waiting_blocked ||
       state.upgrade) {
      return false;
    } else {
      ++state.shared_count;
      state.upgrade = true;
      return true;
    }
  }

  void UnlockUpgrade() {
    MutexLock lk(&state_change);
    state.upgrade = false;
    bool const last_reader=!--state.shared_count;

    if (last_reader) {
      state.exclusive_waiting_blocked = false;
      ReleaseWaiters();
    }
  }

  void UnlockUpgradeAndLock() {
    MutexLock lk(&state_change);
    --state.shared_count;
    while (state.shared_count) {
      upgrade_cond.Wait(&state_change);
    }
    state.upgrade = false;
    state.exclusive = true;
  }

  void UnlockAndLockUpgrade() {
    MutexLock lk(&state_change);
    state.exclusive = false;
    state.upgrade = true;
    ++state.shared_count;
    state.exclusive_waiting_blocked = false;
    ReleaseWaiters();
  }

  void UnlockAndLockShared() {
    MutexLock lk(&state_change);
    state.exclusive = false;
    ++state.shared_count;
    state.exclusive_waiting_blocked = false;
    ReleaseWaiters();
  }

  void UnlockUpgradeAndLockShared() {
    MutexLock lk(&state_change);
    state.upgrade = false;
    state.exclusive_waiting_blocked = false;
    ReleaseWaiters();
  }
};

SharedMutex::SharedMutex()
  : impl_(new Impl) {
}

SharedMutex::~SharedMutex() {
  delete impl_;
}

void SharedMutex::LockShared() {
  impl_->LockShared();
}

bool SharedMutex::TrylockShared() {
  return impl_->TrylockShared();
}

bool SharedMutex::TimedLockShared(
    int time_ms) {
  return impl_->TimedLockShared(time_ms);
}

void SharedMutex::UnlockShared() {
  impl_->UnlockShared();
}

void SharedMutex::Lock() {
  impl_->Lock();
}

bool SharedMutex::TimedLock(int time_ms) {
  return impl_->TimedLock(time_ms);
}

bool SharedMutex::Trylock() {
  return impl_->Trylock();
}

void SharedMutex::Unlock() {
  impl_->Unlock();
}

void SharedMutex::LockUpgrade() {
  impl_->LockUpgrade();
}

bool SharedMutex::TimedLockUpgrade(
    int time_ms) {
  return impl_->TimedLockUpgrade(time_ms);
}

bool SharedMutex::TrylockUpgrade() {
  return impl_->TrylockUpgrade();
}

void SharedMutex::UnlockUpgrade() {
  impl_->UnlockUpgrade();
}

void SharedMutex::UnlockUpgradeAndLock() {
  impl_->UnlockUpgradeAndLock();
}

void SharedMutex::UnlockAndLockShared() {
  impl_->UnlockAndLockShared();
}

void SharedMutex::UnlockUpgradeAndLockShared() {
  return impl_->UnlockUpgradeAndLockShared();
}
