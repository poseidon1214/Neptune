// Copyright (c) 2014 Tencent Inc.
// Author: timcheng

#ifndef COMMON_SYSTEM_TIME_GETTIMESTAMP_H
#define COMMON_SYSTEM_TIME_GETTIMESTAMP_H

#include <stdint.h>
#include <stddef.h>
#include <sys/time.h>

namespace gdt {

inline uint64_t GetTimeStampInMs() {
  struct timeval tv = {0, 0};
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

}

#endif  // COMMON_SYSTEM_TIME_GETTIMESTAMP_H
