// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_CONCURRENCY_BASE_THREAD_H_
#define COMMON_SYSTEM_CONCURRENCY_BASE_THREAD_H_

#include <string>
#include "common/system/concurrency/this_thread.h"
#include "common/system/concurrency/thread_types.h"

namespace gdt {

class Thread;

// BaseThread class is designed to be used as base class,
// and override the Entry virtual method.
class BaseThread {
  friend class Thread;

 public:
  typedef ThreadHandleType HandleType;

 protected:
  BaseThread();
  explicit BaseThread(const ThreadAttributes& attributes);

 public:
  virtual ~BaseThread();

  // Must be called to start a thread, or the thread will not be created.
  // But can't be called again unless joined or detached.
  bool Start();

  // Wait for thread termination
  // the thread must be started and not detached
  bool Join();

  // Whether the thread is still alive
  bool IsAlive() const;

  // Is fitable to call Join?
  bool IsJoinable() const;

  // Get numeric thread id
  int Id() const;

 protected:
  // This virtual function will be called before the thread exiting
  virtual void OnExit();

 private:
  const ThreadAttributes& GetAttributes() {
    return attributes_;
  }

  // BaseThread should not support Detach, because the running thread can't
  // control the memory of BaseThread object. So if it is detached and the
  // BaseThread destructed, the running thread will access the destructed
  // object, make it private.
  bool DoDetach();

  // The derived class must override this function as the thread entry point
  virtual void Entry() = 0;

  static void Cleanup(void* param);

 private:
  static void* StaticEntry(void* inThread);

 private:
  ThreadAttributes attributes_;
  HandleType handle_;
  int id_;
  volatile bool alive_;
};

}  // namespace gdt

#endif  // COMMON_SYSTEM_CONCURRENCY_BASE_THREAD_H_
