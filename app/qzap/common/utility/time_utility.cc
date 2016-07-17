// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5
#include <errno.h>
#include <stdio.h>
#include <string.h>          // for memset()
#include <sys/timeb.h>
#include <time.h>
#include <string>
#include "app/qzap/common/utility/time_utility.h"
#include "app/qzap/common/utility/time_utility_internal.h"
#include "app/qzap/common/base/walltime.h"
#include "thirdparty/glog/logging.h"

void SleepMilliseconds(int milliseconds) {
  if (milliseconds <= 0) return;
  struct timespec sleep_time;
  sleep_time.tv_sec = milliseconds / 1000;
  sleep_time.tv_nsec = (milliseconds % 1000) * 1000000;
  while (nanosleep(&sleep_time, &sleep_time) != 0 && errno == EINTR)
    {}  // Ignore signals and wait for the full interval to elapse.
}

void SleepForSeconds(double seconds) {
  SleepMilliseconds(static_cast<int>(seconds * 1000));
}


static int days_of_month[] = { 0,  // this 0 just makes index start from 1
                               31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static int days_of_month_leap[] = { 0,  // this 0 just makes index start from 1
                             31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool IfLeapYear(int year) {
  // For the sake of completeness, we want to be able to decide
  // whether a year is a leap year all the way back to 0 Julian, or
  // -4713 BCE. But we don't want to take the modulus of a negative
  // number, because this may not be very well-defined or portable. So
  // we increment the year by some large multiple of 400, which is the
  // periodicity of this leap-year calculation.
  if (year < 0)
    year += 8000;
  return ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0));
}

// returns the number of days in the given month, or 0 if month is invalid
int GetDaysInMonth(int y, int m) {
  if ( m < 1 || m > 12 )
    return 0;
  else if ( IfLeapYear(y) )
    return days_of_month_leap[m];
  else
    return days_of_month[m];
}

// return true if this day actually exists
bool IfDayExists(int y, int m, int d) {
  if ( (m <= 0 || m > 12 || d <= 0) ||
       (IfLeapYear(y) && d>days_of_month_leap[m]) ||
       (!IfLeapYear(y) && d>days_of_month[m])
       ) {
    return false;
  }
  return true;
}

// Run on rafferty.nyc (2 X 3400 MHz CPUs).  Run the benchmarks in the unit
// test to get numbers on different processors.
static const int kValueWhereCarefulOvertakesJulian = 160;

bool GetNthNextDay(int n, int *y, int *m, int *d) {
  if ((*m) > 12 || (*m) <= 0 || (*d) > 31 || (*d) <= 0 ||
      (*d) > GetDaysInMonth(*y, *m))
    return false;
  if (n < 0) {
    GetNthPreviousDayCarefully(-n, y, m, d);
  } else {
    GetNthNextDayCarefully(n, y, m, d);
  }
  return true;
}

// returns the time since the Epoch measured in milliseconds
int64_t GetCurrentTimeMillis() {
  return static_cast<int64_t>(WallTime_Now() * 1000);
}

// returns the time since the Epoch measured in microseconds
int64_t GetCurrentTimeMicros() {
  return static_cast<int64_t>(WallTime_Now() * 1000000);
}

// Given a number of seconds representing a time interval, return a std::string
// of the form [-]d:hh:mm:ss, where d is the number of days, hh the number of
// hours, mm the number of minutes, and ss the number of seconds.
std::string TimeIntervalString(int interval) {
  unsigned int i = interval;
  const char *sign = "";
  if (interval < 0) {
    i = -interval;
    sign = "-";
  }

  int s = i % 60;
  i /= 60;
  int m = i % 60;
  i /= 60;
  int h = i % 24;
  int d = i / 24;

  char buf[32];
  snprintf(buf, sizeof(buf), "%s%d:%02d:%02d:%02d", sign, d, h, m, s);
  return buf;
}


// Given a number of seconds representing a time interval, return a std::string
// of the form [-]d:hh:mm:ss, where d is the number of days, hh the number of
// hours, mm the number of minutes, and ss the number of seconds.  Display at
// least num_fields fields, omitting most significant ones if they're zero.
std::string TimeIntervalString(int interval, int num_fields) {
  unsigned int i = interval;
  const char *sign = "";
  if (interval < 0) {
    i = -interval;
    sign = "-";
  }

  int s = i % 60;
  i /= 60;
  int m = i % 60;
  i /= 60;
  int h = i % 24;
  int d = i / 24;

  char buf[32];
  if (d || num_fields >= 4) {
    snprintf(buf, sizeof(buf), "%s%d:%02d:%02d:%02d", sign, d, h, m, s);
  } else if (h || num_fields >= 3) {
    snprintf(buf, sizeof(buf), "%s%d:%02d:%02d", sign, h, m, s);
  } else if (m || num_fields >= 2) {
    snprintf(buf, sizeof(buf), "%s%d:%02d", sign, m, s);
  } else {
    snprintf(buf, sizeof(buf), "%s%d", sign, s);
  }
  return buf;
}


// Same as TimeIntervalString except that the time is given in milliseconds.
// The returned std::string will have the form [-]d:hh:mm:ss.sss, with d, hh, and mm
// optionally omitted if num_fields and the value are both small enough.  The
// three digits of fractional seconds are always returned even when zero.
std::string TimeIntervalStringMS(int64_t interval_ms, int num_fields) {
  uint64_t i = interval_ms;
  const char *sign = "";
  if (interval_ms < 0) {
    i = -interval_ms;
    sign = "-";
  }

  int ms = i % 1000;
  i /= 1000;
  int s = i % 60;
  i /= 60;
  int m = i % 60;
  i /= 60;
  int h = i % 24;
  int d = i / 24;

  char buf[64];
  if (d || num_fields >= 4) {
    snprintf(buf, sizeof(buf), "%s%d:%02d:%02d:%02d.%03d",
             sign, d, h, m, s, ms);
  } else if (h || num_fields >= 3) {
    snprintf(buf, sizeof(buf), "%s%d:%02d:%02d.%03d", sign, h, m, s, ms);
  } else if (m || num_fields >= 2) {
    snprintf(buf, sizeof(buf), "%s%d:%02d.%03d", sign, m, s, ms);
  } else {
    snprintf(buf, sizeof(buf), "%s%d.%03d", sign, s, ms);
  }
  return buf;
}


#define PLURAL(i) (i == 1 ? ' ' : 's')
std::string MakeIntervalString(int interval) {
  char buf[1024];         // giant size for the time-specs we're printing
  if (interval < 60) {
    snprintf(buf, sizeof(buf), "%d sec%c", interval, PLURAL(interval));
    return buf;           // will copy buf into a std::string and then return it
  }

  int r = interval % 60;
  interval = interval / 60;

  if (interval < 60) {
    snprintf(buf, sizeof(buf), "%d min%c %d sec%c",
             interval, PLURAL(interval), r, PLURAL(r));
    return buf;           // will copy buf into a std::string and then return it
  }


  r = interval % 60;
  interval = interval / 60;
  if (interval < 24) {
    snprintf(buf, sizeof(buf), "%d hour%c %d min%c",
             interval, PLURAL(interval), r, PLURAL(r));
    return buf;           // will copy buf into a std::string and then return it
  }

  r = interval % 24;
  interval = interval / 24;
  snprintf(buf, sizeof(buf), "%d day%c %d hour%c",
           interval, PLURAL(interval), r, PLURAL(r));
  return buf;           // will copy buf into a std::string and then return it
}
#undef PLURAL

time_t Time_Today() {
  const time_t now = time(NULL);
  struct tm t;
  localtime_r(&now, &t);
  t.tm_hour = t.tm_min = t.tm_sec = 0;
  time_t today = mktime(&t);
  return today;
}

int GetYearDay() {
  time_t now = time(NULL);
  struct tm t;
  localtime_r(&now, &t);
  return t.tm_yday;
}

int GetDayHour() {
  time_t now = time(NULL);
  struct tm t;
  localtime_r(&now, &t);
  return t.tm_hour;
}

bool FormatTime(
  const std::string &pattern,
  double walltime,
  std::string *output) {
  static int kExternBufferSize = 128;
  struct tm split;
  double subsecond;
  if (!WallTime_Split_Timezone(walltime, true, &split, &subsecond)) {
    LOG(ERROR) << "FormatTime, split time zone fail: " << walltime;
    return false;
  }
  std::string str;
  str.resize(pattern.size() + kExternBufferSize);
  int ret = strftime(&str[0], str.size(), pattern.c_str(),
    &split);
  if (ret > 0) {
    if (output) {
      str.resize(ret);
      *output = str;
    }
    return true;
  }
  LOG(ERROR) << "strftime fail, pattern: " << pattern << " errno:"  << errno << " : " << strerror(errno);
  return false;
}

bool GetTimeFromString(const std::string& formated_time, time_t* t) {
  return GetTimeFromString(formated_time, "%Y%m%d%H%M%S", t);
}
bool GetTimeFromString(const std::string& formated_time,
    const std::string& format,
    time_t* t) {
  if (t == NULL) {
    return false;
  }
  struct tm time_info;
  memset(&time_info, 0, sizeof(time_info));
  const char *p = strptime(formated_time.c_str(), format.c_str(), &time_info);
  if (p == NULL || *p != '\0') {
    LOG(ERROR) << "get time string error time=[" << formated_time << "]"
      << " format =[" << format <<"]";
    return false;
  }

  *t = mktime(&time_info);
  return true;
}


