// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/system/time/posix_time.h"

#ifdef __unix__
#include <stddef.h>
#include <time.h>
#include <sys/time.h>

namespace gdt {

void RelativeTimeInMillSecondsToAbsTimeInTimeSpec(
  int64_t relative_time_in_ms,
  timespec* ts
) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int64_t usec = tv.tv_usec + relative_time_in_ms * 1000LL;
  ts->tv_sec = tv.tv_sec + usec / 1000000;
  ts->tv_nsec = (usec % 1000000) * 1000;
}

} // namespace common

#endif
