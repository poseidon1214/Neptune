// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/system/time/clock.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

TEST(MonotonicClock, MicroSeconds) {
  int64_t t0 = MonotonicClock::MicroSeconds();
  usleep(10000);
  int64_t t1 = MonotonicClock::MicroSeconds();
  ASSERT_GT(t0, 0);
  ASSERT_GT(t1, 0);
  ASSERT_GT(t1, t0);
}

TEST(MonotonicClock, MilliSeconds) {
  int64_t t0 = MonotonicClock::MilliSeconds();
  usleep(10000);
  int64_t t1 = MonotonicClock::MilliSeconds();
  ASSERT_GT(t0, 0);
  ASSERT_GT(t1, 0);
  ASSERT_GT(t1, t0);
}

TEST(MonotonicClock, Unit) {
  int64_t t0 = MonotonicClock::MilliSeconds();
  int64_t t1 = MonotonicClock::MicroSeconds();
  ASSERT_NEAR(1000, t1 / t0, 2);
}

}  // namespace gdt
