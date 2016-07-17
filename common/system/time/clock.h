// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_TIME_CLOCK_H_
#define COMMON_SYSTEM_TIME_CLOCK_H_
#pragma once

#include <stdint.h>

namespace gdt {

// Clock that cannot be set and represents monotonic time since some
// unspecified starting point.
class MonotonicClock {
 public:
  static int64_t MicroSeconds();
  static int64_t MilliSeconds();
};

// System-wide realtime clock.
// can be ajdusted and then skew.
class RealtimeClock {
 public:
  static int64_t MicroSeconds();
  static int64_t MilliSeconds();
};

}  // namespace gdt

#endif  // COMMON_SYSTEM_TIME_CLOCK_H_
