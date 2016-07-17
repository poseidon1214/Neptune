// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5
// Different implementations for GetNthPreviousDay() and GetNthNextDay() with
// different time tradeoffs.  GetNthNextDay chooses which one to use based on N.
// These are in a different compilation unit because they are implementation
// details, but we need to unit test them.

#ifndef APP_QZAP_COMMON_UTILITY_TIME_UTILITY_INTERNAL_H__
#define APP_QZAP_COMMON_UTILITY_TIME_UTILITY_INTERNAL_H__
// The same arguments as GetNthPreviousDay (and GetNthNextDay), but we assume
// the inputs are verified and n is non-negative.  These are implemented by
// iteratively moving forward (or backward) one month at a time until we have
// added (or subtracted) N days.
// These are O(n), but have a small constant.
extern void GetNthPreviousDayCarefully(int n, int *y, int *m, int *d);

extern void GetNthNextDayCarefully(int n, int *y, int *m, int *d);
#endif  // APP_QZAP_COMMON_UTILITY_TIME_UTILITY_INTERNAL_H__
