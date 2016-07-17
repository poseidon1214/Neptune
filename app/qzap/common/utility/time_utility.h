// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5
#ifndef APP_QZAP_COMMON_UTILITY_TIME_UTILITY_H_
#define APP_QZAP_COMMON_UTILITY_TIME_UTILITY_H_

#include <stdint.h>
#include <string>
// Some useful constants
static const int kNumSecondsPerHour = 3600;
static const int kNumSecondsPerDay = kNumSecondsPerHour * 24;
static const int kNumSecondsPerWeek = kNumSecondsPerDay * 7;

// Sleeps for "milliseconds" ms
extern void SleepMilliseconds(int milliseconds);

extern void SleepForSeconds(double seconds);

// Returns false and does nothing if inputs are invalid, otherwise true.
// (y, m, d) are in/out parameters, where y is anything, *m=[1..12], *d=[1..31].
// They will be modified only if they are valid.
// n is any integer, and is the days offset from y-m-d.
extern bool GetNthNextDay(int n, int *y, int *m, int *d);

// These three are all convenience function around GetNthNextDay().
inline bool GetNthPreviousDay(int n, int *y, int *m, int *d) {
  return GetNthNextDay(-n, y, m, d);
}

inline bool GetNextDay(int *y, int *m, int *d) {
  return GetNthNextDay(1, y, m, d);
}

inline bool GetPreviousDay(int *y, int *m, int *d) {
  return GetNthPreviousDay(1, y, m, d);
}

// return true if the given year is a leap year
extern bool IfLeapYear(int year);

// return true if this day does exist (m=[1..12], d=[1..31]).
extern bool IfDayExists(int y, int m, int d);

// return true if the given year is a leap year
extern bool IfLeapYear(int year);

// returns the number of days in the given month, or 0 if month is invalid
// (m=[1..12])
extern int GetDaysInMonth(int y, int m);

// returns the time since the Epoch measured in milliseconds
extern int64_t GetCurrentTimeMillis();

// returns the time since the Epoch measured in microseconds
extern int64_t GetCurrentTimeMicros();

// Given a number of seconds representing a time interval, return a std::string
// of the form [-]d:hh:mm:ss, where d is the number of days, hh the number of
// hours, mm the number of minutes, and ss the number of seconds.  If num_fields
// is given, display at least that many fields; if omitted, display all of them
// even if they're zero.
std::string TimeIntervalString(int interval, int num_fields);
std::string TimeIntervalString(int interval);
inline std::string TimeIntervalString(double interval) {
  return TimeIntervalString(static_cast<int>(interval), 4);
}

// Same as TimeIntervalString except that the time is given in milliseconds.
// The returned std::string will have the form [-]d:hh:mm:ss.sss, with d, hh, and mm
// optionally omitted if num_fields and the value are both small enough.  The
// three digits of fractional seconds are always returned even when zero.
std::string TimeIntervalStringMS(int64_t interval_ms, int num_fields);

// Given a number of seconds representing a time interval, return a std::string
// that is a human-readable description of that interval.  For example,
// given '90', returns '1 min 30 secs'.
extern std::string MakeIntervalString(int interval_secs);

// Format time acording to pattern.
// e.g., pattern is 'now time is mm-hh' will be formatted to
// 'now time is minutes-hours'
// man strftime to see the format string.
bool FormatTime(const std::string &pattern, double walltime, std::string *output);

time_t Time_Today();
int GetYearDay();
int GetDayHour();

// defaut formated is "%Y%m%d%H%M%S"
bool GetTimeFromString(const std::string& formated_time, time_t* t);
bool GetTimeFromString(const std::string& formated_time, const std::string& format, time_t* t);
#endif  // APP_QZAP_COMMON_UTILITY_TIME_UTILITY_H_
