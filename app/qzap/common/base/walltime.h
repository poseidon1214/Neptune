// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17
// use clock_gettime, keep the same interface
// WallTime is the number of seconds since the Unix epoch.  We represent
// this as a double so we can keep track of fractional seconds.

#ifndef APP_QZAP_COMMON_BASE_WALLTIME_H_
#define APP_QZAP_COMMON_BASE_WALLTIME_H_

#include <string>
typedef double WallTime;
WallTime WallTime_Now();
// Convert "time" into a "string".  More convenient form of WallTime_Print().
std::string WallTime_ToString(WallTime time);
bool WallTime_Split_Timezone(
  WallTime value, bool local,
  struct tm* t, double* subsecond);
#endif  // APP_QZAP_COMMON_BASE_WALLTIME_H_
