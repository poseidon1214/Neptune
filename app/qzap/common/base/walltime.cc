// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17
// use clock_gettime, keep the same interface

#include "app/qzap/common/base/walltime.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <limits>
#include <numeric>
#include <string>

// ------------------------------------------------------------------------

// The printing support produces strings of the form:
//      yyyy/mm/dd-hh:mm:ss.ms
//      2004/07/13-14:23:37.697
// The printed form is in local time.
//
// The printed output will be incorrect if the time is not within the
// range [0, kuint32max].
//
// If you want a different format, use the WallTime_Split() method
// above and do your own formatting.

// Temporary storage for printing output
struct WallTime_PrintBuffer {
  char buffer[32];      // Enough space for format given above
};

// return seconds
WallTime WallTime_Now() {
  WallTime realtime = 0.0f;
  timespec time_now;
  clock_gettime(CLOCK_REALTIME, &time_now);
  realtime = time_now.tv_sec + time_now.tv_nsec / 1000000000.0;
  return realtime;
}

bool WallTime_Split_Timezone(
  WallTime value, bool local,
  struct tm* t, double* subsecond) {
  if ((value < 0) || (value > std::numeric_limits<uint32_t>::max())) {
    if (subsecond != NULL) {
      *subsecond = 0.0;
    }
    return false;
  }
  if (t == NULL) {
    return true;
  }
  memset(t, 0, sizeof(*t));
  const uint32_t whole = static_cast<uint32_t>(value);
  const time_t whole_time = whole;
  if (subsecond != NULL) {
    *subsecond = value - whole;
  }
  if (local) {
    localtime_r(&whole_time, t);
  } else {
    gmtime_r(&whole_time, t);
  }
  return true;
}

const char* WallTime_Print_Timezone(
  WallTime time, bool local,
  WallTime_PrintBuffer* storage) {
  struct tm split;
  double subsecond;
  WallTime_Split_Timezone(time, local, &split, &subsecond);
  int ms = static_cast<int>((subsecond * 1000) + 0.5);
  if (ms > 999) ms = 999;      // Do not overflow
  if (ms < 0)   ms = 0;        // Do not underflow
  snprintf(storage->buffer, sizeof(storage->buffer),
    "%04d/%02d/%02d-%02d:%02d:%02d.%03d",
     split.tm_year + 1900,
     split.tm_mon + 1,
     split.tm_mday,
     split.tm_hour,
     split.tm_min,
     split.tm_sec,
     ms);
  return storage->buffer;
}

std::string WallTime_ToString_Timezone(WallTime time, bool local) {
  WallTime_PrintBuffer storage;
  return std::string(WallTime_Print_Timezone(time, local, &storage));
}

// The original WallTime_ToString now calls the generic one.
std::string WallTime_ToString(WallTime time) {
  return WallTime_ToString_Timezone(time, true);
}
