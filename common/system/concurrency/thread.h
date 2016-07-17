// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2010-06-18

#ifndef COMMON_SYSTEM_CONCURRENCY_THREAD_H_
#define COMMON_SYSTEM_CONCURRENCY_THREAD_H_

#include <string>
#include "app/qzap/common/base/callback.h"
#include "common/base/uncopyable.h"
#include "common/system/concurrency/thread_types.h"

namespace gdt {

// Thread 类
// 由用户直接传入线程函数使用
class Thread {
  DECLARE_UNCOPYABLE(Thread);
  class Impl;
 public:
  // NULL Closure, 必须调 Initialize 后才能 Start
  explicit Thread(const ThreadAttributes& attributes, Closure* closure = NULL);
  explicit Thread(const std::string& name, Closure* closure = NULL);
  explicit Thread(Closure* closure = NULL);

  ~Thread();

  // 初始化线程对象
  void Initialize(Closure* closure);

  // 只能在 Start 之前调用
  void SetAffinity(int cpu_index) __attribute__((deprecated));
  void SetDetach() __attribute__((deprecated));

  // 必须调用 Start 后才会真正启动
  bool Start();

  // 等待正在运行的线程结束
  // 只有线程已经运行了，且没有 Detach，才能 Join
  // 如果线程已经结束，立即返回 true
  bool Join();

  // 把实际线程和线程对象分离，Detach 之后，不能再通过任何函数访问到实际线程
  void Detach();

  // 返回线程是否还在存活
  bool IsAlive() const;

  // 返回是否可以对线程调 Join
  bool IsJoinable() const;

  // 获得线程ID（类似PID的整数）
  int Id() const;

 private:
  void Entry() const {}

 private:
  Impl* pimpl_;
};

}  // namespace gdt

#endif  // COMMON_SYSTEM_CONCURRENCY_THREAD_H_

