// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5
#include "app/qzap/common/base/benchmark.h"
#include "app/qzap/common/utility/time_utility.h"
#include "app/qzap/common/utility/time_utility_internal.h"
#include "app/qzap/common/base/walltime.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

class TimeUtilityTest : public testing::Test {
};

TEST_F(TimeUtilityTest, SleepMilliSeconds) {
  // first test an uninterrupted sleep.
  VLOG(0) << "testing uninterrupted SleepMilliseconds";
  WallTime start = WallTime_Now();
  SleepMilliseconds(2500);
  WallTime end = WallTime_Now();
  EXPECT_LE(2.4, end - start);
  EXPECT_GE(2.6, end - start);
}

TEST_F(TimeUtilityTest, SleepForSeconds) {
  WallTime start = WallTime_Now();
  SleepForSeconds(2.5);
  WallTime end = WallTime_Now();
  EXPECT_LE(2.4, end - start);
  EXPECT_GE(2.6, end - start);
}

TEST_F(TimeUtilityTest, GetDaysInMonth) {
  CHECK_EQ(GetDaysInMonth(1900, -1), 0);
  CHECK_EQ(GetDaysInMonth(1900, 0), 0);
  CHECK_EQ(GetDaysInMonth(1900, 1), 31);
  CHECK_EQ(GetDaysInMonth(1900, 2), 28);    // feb in non-LY
  CHECK_EQ(GetDaysInMonth(1904, 2), 29);    // feb in LY
  CHECK_EQ(GetDaysInMonth(2000, 2), 29);    // feb in LY
  CHECK_EQ(GetDaysInMonth(1900, 3), 31);
  CHECK_EQ(GetDaysInMonth(1900, 4), 30);
  CHECK_EQ(GetDaysInMonth(1900, 5), 31);
  CHECK_EQ(GetDaysInMonth(1900, 6), 30);
  CHECK_EQ(GetDaysInMonth(1900, 7), 31);
  CHECK_EQ(GetDaysInMonth(1900, 8), 31);
  CHECK_EQ(GetDaysInMonth(1900, 9), 30);
  CHECK_EQ(GetDaysInMonth(1900, 10), 31);
  CHECK_EQ(GetDaysInMonth(1900, 11), 30);
  CHECK_EQ(GetDaysInMonth(1900, 12), 31);
  CHECK_EQ(GetDaysInMonth(1900, 13), 0);
}

TEST_F(TimeUtilityTest, TestIfDayExists) {
  // illegal m/d
  CHECK(!IfDayExists(1904, 13, 29));
  CHECK(!IfDayExists(1904, 0, 29));
  CHECK(!IfDayExists(1904, 2, -12));

  // check expected values
  CHECK(!IfDayExists(1900, 2, 29));
  CHECK(IfDayExists(1900, 2, 28));
  CHECK(IfDayExists(1904, 2, 29));
  CHECK(IfDayExists(2000, 2, 29));
}

static void BM_GetNthNextDayCarefully(int iters, int size) {
  for (int i = 0; i < iters; ++i) {
    int d = 1, m = 1, y = 1900;
    GetNthNextDayCarefully(size, &y, &m, &d);
  }
}

// Find the N where the careful method is slower than the Julian method.
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 1);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 8);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 64);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 128);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 160);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 192);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 224);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 256);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 288);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 320);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 352);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 384);
BENCHMARK_WITH_ARG(BM_GetNthNextDayCarefully, 512);

static void TestGetNthNextDay(void (* get)(int, int*, int*, int*)) {
  int d = 1, m = 1, y = 1900;

  // normal day increment
  (*get)(1, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 2);

  // roll over month once
  d = 1;
  m = 1;
  y = 1900;
  (*get)(31, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 2);
  EXPECT_EQ(d, 1);

  // roll over month three times
  d = 1;
  m = 1;
  y = 1900;
  (*get)(91, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 4);
  EXPECT_EQ(d, 2);

  // roll over month and year
  d = 1;
  m = 1;
  y = 1900;
  (*get)(400, &y, &m, &d);
  EXPECT_EQ(y, 1901);
  EXPECT_EQ(m, 2);
  EXPECT_EQ(d, 5);

  // roll over month and multiple years
  d = 1;
  m = 1;
  y = 1900;
  (*get)(1200, &y, &m, &d);
  EXPECT_EQ(y, 1903);
  EXPECT_EQ(m, 4);
  EXPECT_EQ(d, 16);

  // roll over from the middle of the month and year
  d = 23;
  m = 8;
  y = 2006;
  (*get)(1200, &y, &m, &d);
  EXPECT_EQ(y, 2009);
  EXPECT_EQ(m, 12);
  EXPECT_EQ(d, 5);

  // check non-leap year
  d = 28;
  m = 2;
  y = 1900;
  (*get)(1, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 3);
  EXPECT_EQ(d, 1);

  // check leap year
  d = 28;
  m = 2;
  y = 2000;
  (*get)(1, &y, &m, &d);
  EXPECT_EQ(y, 2000);
  EXPECT_EQ(m, 2);
  EXPECT_EQ(d, 29);

  // check negative N
  d = 2;
  m = 1;
  y = 1900;
  (*get)(-1, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);

  // check null version
  d = 1;
  m = 1;
  y = 1900;
  (*get)(0, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);

  // But what about negative years?
}

TEST_F(TimeUtilityTest, GetNthNextDay) {
  TestGetNthNextDay(&GetNthNextDayCarefully);
  // check null version
  int d = 1, m = 1, y = 1900;
  CHECK(GetNthNextDay(0, &y, &m, &d));
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);

  // check invalid day (1900 is not a leap year)
  d = 29;
  m = 2;
  y = 1900;
  CHECK(!GetNthNextDay(1, &y, &m, &d));

  // more invalid cases
  d = 0;
  m = 2;
  y = 1900;
  CHECK(!GetNthNextDay(1, &y, &m, &d));
  d = 32;
  m = 2;
  y = 1900;
  CHECK(!GetNthNextDay(1, &y, &m, &d));
  d = 1;
  m = 0;
  y = 1900;
  CHECK(!GetNthNextDay(1, &y, &m, &d));
  d = 1;
  m = 13;
  y = 1900;
  CHECK(!GetNthNextDay(1, &y, &m, &d));
}

TEST_F(TimeUtilityTest, GetNextDay) {
  int d = 1, m = 1, y = 1900;

  // normal day increment
  CHECK(GetNextDay(&y, &m, &d));
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 2);

  // roll over month
  d = 31;
  m = 1;
  y = 1900;
  CHECK(GetNextDay(&y, &m, &d));
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 2);
  EXPECT_EQ(d, 1);

  // roll over month and year
  d = 31;
  m = 12;
  y = 1900;
  CHECK(GetNextDay(&y, &m, &d));
  EXPECT_EQ(y, 1901);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);

  // check non-leap year
  d = 28;
  m = 2;
  y = 1900;
  CHECK(GetNextDay(&y, &m, &d));
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 3);
  EXPECT_EQ(d, 1);

  // check leap year
  d = 28;
  m = 2;
  y = 2000;
  CHECK(GetNextDay(&y, &m, &d));
  EXPECT_EQ(y, 2000);
  EXPECT_EQ(m, 2);
  EXPECT_EQ(d, 29);

  // check invalid day (1900 is not a leap year)
  d = 29;
  m = 2;
  y = 1900;
  CHECK(!GetNextDay(&y, &m, &d));

  // more invalid cases
  d = 0;
  m = 2;
  y = 1900;
  CHECK(!GetNextDay(&y, &m, &d));
  d = 32;
  m = 2;
  y = 1900;
  CHECK(!GetNextDay(&y, &m, &d));
  d = 1;
  m = 0;
  y = 1900;
  CHECK(!GetNextDay(&y, &m, &d));
  d = 1;
  m = 13;
  y = 1900;
  CHECK(!GetNextDay(&y, &m, &d));

  // But what about negative years?
}

static void TestGetNthPreviousDay(void (* get)(int, int*, int*, int*)) {
  int d = 2, m = 1, y = 1900;

  // normal day increment
  (*get)(1, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);

  // roll over month once
  d = 1;
  m = 2;
  y = 1900;
  (*get)(31, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);

  // roll over month three times
  d = 2;
  m = 4;
  y = 1900;
  (*get)(91, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);

  // roll over month and year
  d = 5;
  m = 2;
  y = 1901;
  (*get)(400, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);

  // roll over month and multiple years
  d = 16;
  m = 4;
  y = 1903;
  (*get)(1200, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);

  // roll over from the middle of the month and year
  d = 5;
  m = 12;
  y = 2009;
  (*get)(1200, &y, &m, &d);
  EXPECT_EQ(y, 2006);
  EXPECT_EQ(m, 8);
  EXPECT_EQ(d, 23);

  // check negative N
  d = 1;
  m = 1;
  y = 1900;
  (*get)(-1, &y, &m, &d);
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 2);

  // But what about negative years?
}

TEST_F(TimeUtilityTest, GetNthPreviousDay) {
  TestGetNthPreviousDay(&GetNthPreviousDayCarefully);
  // check invalid day (1900 is not a leap year)
  int d = 29, m = 2, y = 1900;
  CHECK(!GetNthPreviousDay(1, &y, &m, &d));

  // more invalid cases
  d = 0;
  m = 2;
  y = 1900;
  CHECK(!GetNthPreviousDay(1, &y, &m, &d));
  d = 32;
  m = 2;
  y = 1900;
  CHECK(!GetNthPreviousDay(1, &y, &m, &d));
  d = 1;
  m = 0;
  y = 1900;
  CHECK(!GetNthPreviousDay(1, &y, &m, &d));
  d = 1;
  m = 13;
  y = 1900;
  CHECK(!GetNthPreviousDay(1, &y, &m, &d));

  // check null version
  d = 1;
  m = 1;
  y = 1900;
  CHECK(GetNthPreviousDay(0, &y, &m, &d));
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);
}

static void TestGetNthDay(void (* next)(int, int*, int*, int*),
                          void (* prev)(int, int*, int*, int*)) {
  // Test next and previous for 10000 days starting with 01/01/1900, for 400
  // days
  int d = 1, m = 1, y = 1900;
  int nd = 0;
  int nm = 0;
  int ny = 0;
  int pd = 0;
  int pm = 0;
  int py = 0;
  for (int i = 0; i < 10000; ++i) {
    for (int n = 1; n < 400; ++n) {
      nd = d;
      nm = m;
      ny = y;
      (*next)(n, &ny, &nm, &nd);
      pd = nd;
      pm = nm;
      py = ny;
      (*prev)(n, &py, &pm, &pd);
      EXPECT_EQ(pd, d);
      EXPECT_EQ(pm, m);
      EXPECT_EQ(py, y);
    }
    d = nd;
    m = nm;
    y = ny;
  }
}

TEST_F(TimeUtilityTest, GetNthDay) {
  TestGetNthDay(&GetNthNextDayCarefully,
                &GetNthPreviousDayCarefully);
}

TEST_F(TimeUtilityTest, GetPreviousDay) {
  LOG(INFO) << "TestGetPreviousDay";
  int d = 2, m = 1, y = 1900;

  // normal day decrement
  CHECK(GetPreviousDay(&y, &m, &d));
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 1);

  // roll over month
  d = 1;
  m = 2;
  y = 1900;
  CHECK(GetPreviousDay(&y, &m, &d));
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 1);
  EXPECT_EQ(d, 31);

  // roll over month and year
  d = 1;
  m = 1;
  y = 1901;
  CHECK(GetPreviousDay(&y, &m, &d));
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 12);
  EXPECT_EQ(d, 31);

  // check non-leap year
  d = 1;
  m = 3;
  y = 1900;
  CHECK(GetPreviousDay(&y, &m, &d));
  EXPECT_EQ(y, 1900);
  EXPECT_EQ(m, 2);
  EXPECT_EQ(d, 28);

  // check leap year
  d = 1;
  m = 3;
  y = 2000;
  CHECK(GetPreviousDay(&y, &m, &d));
  EXPECT_EQ(y, 2000);
  EXPECT_EQ(m, 2);
  EXPECT_EQ(d, 29);

  // check invalid day (1900 is not a leap year)
  d = 29;
  m = 2;
  y = 1900;
  CHECK(!GetPreviousDay(&y, &m, &d));

  // more invalid cases
  d = 0;
  m = 2;
  y = 1900;
  CHECK(!GetPreviousDay(&y, &m, &d));
  d = 32;
  m = 2;
  y = 1900;
  CHECK(!GetPreviousDay(&y, &m, &d));
  d = 1;
  m = 0;
  y = 1900;
  CHECK(!GetPreviousDay(&y, &m, &d));
  d = 1;
  m = 13;
  y = 1900;
  CHECK(!GetPreviousDay(&y, &m, &d));

  // Test next and previous for 100000 days starting with 01/01/1900
  d = 1;
  m = 1;
  y = 1900;
  int nd = 0;
  int nm = 0;
  int ny = 0;
  int pd = 0;
  int pm = 0;
  int py = 0;
  for (int i = 0; i < 100000; ++i) {
    nd = d;
    nm = m;
    ny = y;
    CHECK(GetNextDay(&ny, &nm, &nd));
    pd = nd;
    pm = nm;
    py = ny;
    CHECK(GetPreviousDay(&py, &pm, &pd));
    EXPECT_EQ(pd, d);
    EXPECT_EQ(pm, m);
    EXPECT_EQ(py, y);
    d = nd;
    m = nm;
    y = ny;
  }

  // But what about negative years?
}

TEST_F(TimeUtilityTest, TimeIntervalString) {
  LOG(INFO) << "TestTimeIntervalString";

  CHECK_EQ(TimeIntervalString(0), "0:00:00:00");
  CHECK_EQ(TimeIntervalString(0, 4), "0:00:00:00");
  CHECK_EQ(TimeIntervalString(0, 3), "0:00:00");
  CHECK_EQ(TimeIntervalString(0, 2), "0:00");
  CHECK_EQ(TimeIntervalString(0, 1), "0");
  CHECK_EQ(TimeIntervalString(0, 0), "0");
  CHECK_EQ(TimeIntervalString(-1), "-0:00:00:01");
  CHECK_EQ(TimeIntervalString(-1, 4), "-0:00:00:01");
  CHECK_EQ(TimeIntervalString(-1, 3), "-0:00:01");
  CHECK_EQ(TimeIntervalString(-1, 2), "-0:01");
  CHECK_EQ(TimeIntervalString(-1, 1), "-1");
  CHECK_EQ(TimeIntervalString(-1, 0), "-1");
  CHECK_EQ(TimeIntervalString(17), "0:00:00:17");
  CHECK_EQ(TimeIntervalString(17, 4), "0:00:00:17");
  CHECK_EQ(TimeIntervalString(17, 3), "0:00:17");
  CHECK_EQ(TimeIntervalString(17, 2), "0:17");
  CHECK_EQ(TimeIntervalString(17, 1), "17");
  CHECK_EQ(TimeIntervalString(17, 0), "17");
  CHECK_EQ(TimeIntervalString(-60, 0), "-1:00");
  CHECK_EQ(TimeIntervalString(7201), "0:02:00:01");
  CHECK_EQ(TimeIntervalString(7201, 4), "0:02:00:01");
  CHECK_EQ(TimeIntervalString(7201, 3), "2:00:01");
  CHECK_EQ(TimeIntervalString(7201, 2), "2:00:01");
  CHECK_EQ(TimeIntervalString(7201, 0), "2:00:01");
  CHECK_EQ(TimeIntervalString(-7201), "-0:02:00:01");
  CHECK_EQ(TimeIntervalString(-7201, 4), "-0:02:00:01");
  CHECK_EQ(TimeIntervalString(-7201, 3), "-2:00:01");
  CHECK_EQ(TimeIntervalString(-7201, 2), "-2:00:01");
  CHECK_EQ(TimeIntervalString(-7201, 0), "-2:00:01");
  CHECK_EQ(TimeIntervalString(1000000), "11:13:46:40");
  CHECK_EQ(TimeIntervalString(1000000, 5), "11:13:46:40");
  CHECK_EQ(TimeIntervalString(1000000, 3), "11:13:46:40");
  CHECK_EQ(TimeIntervalString(-1000000, 3), "-11:13:46:40");
  CHECK_EQ(TimeIntervalString(2147483647, 0), "24855:03:14:07");
  CHECK_EQ(TimeIntervalString(static_cast<int>(2147483648U), 0),
           "-24855:03:14:08");

  CHECK_EQ(TimeIntervalStringMS(0, 4), "0:00:00:00.000");
  CHECK_EQ(TimeIntervalStringMS(0, 3), "0:00:00.000");
  CHECK_EQ(TimeIntervalStringMS(0, 2), "0:00.000");
  CHECK_EQ(TimeIntervalStringMS(0, 1), "0.000");
  CHECK_EQ(TimeIntervalStringMS(0, 0), "0.000");
  CHECK_EQ(TimeIntervalStringMS(3200, 0), "3.200");
  CHECK_EQ(TimeIntervalStringMS(-1, 4), "-0:00:00:00.001");
  CHECK_EQ(TimeIntervalStringMS(-1, 3), "-0:00:00.001");
  CHECK_EQ(TimeIntervalStringMS(-1, 2), "-0:00.001");
  CHECK_EQ(TimeIntervalStringMS(-1, 1), "-0.001");
  CHECK_EQ(TimeIntervalStringMS(-1, 0), "-0.001");
  CHECK_EQ(TimeIntervalStringMS(17320, 4), "0:00:00:17.320");
  CHECK_EQ(TimeIntervalStringMS(17320, 3), "0:00:17.320");
  CHECK_EQ(TimeIntervalStringMS(17320, 2), "0:17.320");
  CHECK_EQ(TimeIntervalStringMS(17320, 1), "17.320");
  CHECK_EQ(TimeIntervalStringMS(17320, 0), "17.320");
  CHECK_EQ(TimeIntervalStringMS(-120005, 0), "-2:00.005");
  CHECK_EQ(TimeIntervalStringMS(7201925, 4), "0:02:00:01.925");
  CHECK_EQ(TimeIntervalStringMS(7201925, 3), "2:00:01.925");
  CHECK_EQ(TimeIntervalStringMS(7201922, 2), "2:00:01.922");
  CHECK_EQ(TimeIntervalStringMS(7201925, 0), "2:00:01.925");
  CHECK_EQ(TimeIntervalStringMS(1000000002, 5), "11:13:46:40.002");
  CHECK_EQ(TimeIntervalStringMS(1000000999, 3), "11:13:46:40.999");
  CHECK_EQ(TimeIntervalStringMS(-1000001321, 3), "-11:13:46:41.321");
  CHECK_EQ(TimeIntervalStringMS(12345678912345678LL, 0),
           "142889802:05:25:45.678");
}

TEST_F(TimeUtilityTest, MakeIntervalString) {
  LOG(INFO) << "TestMakeIntervalString()";

  CHECK_EQ("1 sec ", MakeIntervalString(1));
  CHECK_EQ("59 secs", MakeIntervalString(59));

  CHECK_EQ("1 min  0 secs", MakeIntervalString(60));
  CHECK_EQ("1 min  1 sec ", MakeIntervalString(61));
  CHECK_EQ("2 mins 59 secs", MakeIntervalString((2 * 60) + 59));
  CHECK_EQ("59 mins 7 secs", MakeIntervalString((59 * 60) + 7));

  CHECK_EQ("1 hour  2 mins", MakeIntervalString((1 * 3600) + 2 * 60));
  CHECK_EQ("2 hours 1 min ", MakeIntervalString((2 * 3600) + 1 * 60));

  CHECK_EQ("1 day  12 hours", MakeIntervalString(86400 + (12 * 3600)));
  CHECK_EQ("2 days 1 hour ", MakeIntervalString((2 * 86400) + (1 * 3600)));
}

TEST_F(TimeUtilityTest, GetCurrentTimeMicrosMonotonic) {
  LOG(INFO) << "TestGetCurrentTimeMicrosMonotonic()";
  const int64_t start = GetCurrentTimeMicros();
  int64_t now = start;
  int64_t last = start;
  int errors = 0;

  while (now - start < 15 * 1000 * 1000) {
    now = GetCurrentTimeMicros();
    if (now < last) {
      LOG(ERROR) << "Regression noted, now = " << now << ", last = " << last
                 << ", delta = " << (last - now);
      ++errors;
    }
    last = now;
  }
  EXPECT_EQ(0, errors);
}

TEST_F(TimeUtilityTest, TestToday) {
  LOG(INFO) << "Checking WallTime_Today()";
  const time_t now1 = time(NULL);
  struct tm t;
  localtime_r(&now1, &t);
  t.tm_hour = t.tm_min = t.tm_sec = 0;
  time_t today = mktime(&t);
  LOG(INFO) << Time_Today();
  ASSERT_EQ(today, Time_Today());
}

TEST_F(TimeUtilityTest, TestGetYearDay) {
  LOG(INFO) << "Checking WallTime_Today()";
  const time_t now1 = time(NULL);
  struct tm t;
  localtime_r(&now1, &t);
  ASSERT_EQ(t.tm_yday, GetYearDay());
  ASSERT_EQ(t.tm_hour, GetDayHour());
}

TEST_F(TimeUtilityTest, TestFormatTime) {
  LOG(INFO) << "Checking FormatTime";
  struct tm t;
  t.tm_hour = t.tm_min = t.tm_sec = 1;
  t.tm_year = 2013 - 1900;
  t.tm_mon = 2;
  t.tm_mday = 6;
  time_t today = mktime(&t);
  std::string format;
  ASSERT_TRUE(FormatTime("markdown time is %Y-%m-%d-%H-%M-%S", static_cast<double>(today), &format));
  ASSERT_EQ(format, "markdown time is 2013-03-06-01-01-01");
}
