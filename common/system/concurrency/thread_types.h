// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/31/11

#ifndef COMMON_SYSTEM_CONCURRENCY_THREAD_TYPES_H_
#define COMMON_SYSTEM_CONCURRENCY_THREAD_TYPES_H_
#pragma once

#include <errno.h>
#include <pthread.h>
#include <stdint.h>

#include <string>


namespace gdt {

typedef pthread_t   ThreadHandleType;

class BaseThread;

/// ThreadAttributes represent thread attribute.
/// Usage:
/// ThreadAttributes()
///     .SetName("ThreadPoolThread")
///     .SetStackSize(64 * 1024)
class ThreadAttributes {
  friend class BaseThread;
 public:
  ThreadAttributes();
  ~ThreadAttributes();
  ThreadAttributes& SetName(const std::string& name);
  ThreadAttributes& SetStackSize(size_t size);
  ThreadAttributes& SetDetached(bool detached);
  ThreadAttributes& SetPriority(int priority);
  ThreadAttributes& SetAffinity(int cpu_index);
  bool IsDetached() const;
  uint64_t AffinityMask() const { return affinity_mask_; }
 private:
  std::string name_;
  pthread_attr_t attr_;
  uint64_t affinity_mask_;
};

}  // namespace gdt

#endif  // COMMON_SYSTEM_CONCURRENCY_THREAD_TYPES_H_
