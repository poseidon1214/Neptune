// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_TIME_POSIX_TIME_H
#define COMMON_SYSTEM_TIME_POSIX_TIME_H

#include <stdint.h>
struct timespec;

namespace gdt {

// for any timed* functions using absolute timespec
void RelativeTimeInMillSecondsToAbsTimeInTimeSpec(
  int64_t relative_time_in_ms,
  timespec* ts
);

} // namespace common

#endif // COMMON_SYSTEM_TIME_POSIX_TIME_H
