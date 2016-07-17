// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
#ifndef PLATFORM_THREAD_COUNT_BLOCKER_H_
#define PLATFORM_THREAD_COUNT_BLOCKER_H_
#include "app/qzap/common/thread/mutex.h"
// A BlockingCounter allows a thread to wait for a pre-specified number of
// actions to occur.
class CountBlocker {
 public:
  explicit CountBlocker(int64_t initial_count)
      : count_(initial_count) {
  }

  void Dec(int64_t cnt) {
    MutexLock locker(&mutex_);
    count_ -= cnt;
    if (count_ == 0) {
      notify_.NotifyAll();
    }
  }

  void Inc(int64_t cnt) {
    MutexLock locker(&mutex_);
    count_ += cnt;
  }

  // It is not the actually count because access without lock, it is only hint.
  int64_t count_hint() const {
    return count_;
  }

  // Return true when notified, otherwise return false.
  bool Wait() {
    return Wait(std::numeric_limits<int32_t>::max());
  }
  bool Wait(int timeout_ms) {
    MutexLock locker(&mutex_);
    while (count_ > 0) {
      if (notify_.TimedWait(&mutex_, timeout_ms)) {
        return true;
      } else {
        return false;
      }
    }
    return true;
  }
 private:
  int64_t count_;
  Mutex mutex_;
  CondVar notify_;
  DISALLOW_COPY_AND_ASSIGN(CountBlocker);
};
#endif  // PLATFORM_THREAD_COUNT_BLOCKER_H_
