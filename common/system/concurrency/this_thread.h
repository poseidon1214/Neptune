// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/31/11
// Description: current thread scoped attributes and operations

#ifndef COMMON_SYSTEM_CONCURRENCY_THIS_THREAD_H
#define COMMON_SYSTEM_CONCURRENCY_THIS_THREAD_H
#pragma once

#include <stdint.h>
#include "common/system/concurrency/thread_types.h"

namespace gdt {

/// thread scoped attribute and operations of current thread
class ThisThread {
  ThisThread();
  ~ThisThread();
 public:
  static void Exit();
  static void Yield();
  static void Sleep(int64_t time_in_ms);
  static int GetLastErrorCode();
  static ThreadHandleType GetHandle();
  static int GetId();
  static bool IsMain();
};

} // namespace gdt

#endif // COMMON_SYSTEM_CONCURRENCY_THIS_THREAD_H
