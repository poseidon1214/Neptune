// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17
#include <math.h>
#include <sys/time.h>
#include <syscall.h>  // for __NR_gettimeofday
#include <time.h>
#include <unistd.h>
#include <iostream>
#include "app/qzap/common/base/walltime.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

class WallTimeTest: public testing::Test {
};

TEST_F(WallTimeTest, TestNow) {
  LOG(INFO) << "Checking WallTime_Now()";
  const time_t now1 = time(NULL);
  const WallTime now2 = WallTime_Now();
  ASSERT_GE(now2, (WallTime) (now1 - 2));
  ASSERT_LE(now2, (WallTime) (now1 + 2));
}

TEST_F(WallTimeTest, TestSplit) {
  struct tm t;
  t.tm_hour = t.tm_min = t.tm_sec = 1;
  t.tm_year = 2013 - 1900;
  t.tm_mon = 2;
  t.tm_mday = 6;
  time_t today = mktime(&t);
  struct tm t2;
  ASSERT_TRUE(WallTime_Split_Timezone(
    static_cast<WallTime>(today), true,
    NULL, NULL));
  ASSERT_TRUE(WallTime_Split_Timezone(
    static_cast<WallTime>(today), true,
    &t2, NULL));
  ASSERT_EQ(t.tm_hour, t2.tm_hour);
  ASSERT_EQ(t.tm_min, t2.tm_min);
  ASSERT_EQ(t.tm_sec, t2.tm_sec);
  ASSERT_EQ(t.tm_year, t2.tm_year);
  ASSERT_EQ(t.tm_mon, t2.tm_mon);
  ASSERT_EQ(t.tm_mday, t2.tm_mday);
}
