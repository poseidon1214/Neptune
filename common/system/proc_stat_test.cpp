// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Wed Aug  5 17:48:58 CST 2015

#include "common/system/proc_stat.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

namespace gdt {

class ProcStatTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(ProcStatTest, GetProcStat) {
  CpuJiffies cpu_jiffies;
  ASSERT_TRUE(GetProcStat(&cpu_jiffies));
}

TEST_F(ProcStatTest, GetSumJiffies) {
  int64_t sum = GetSumJiffies();
  ASSERT_GT(sum, 0);
  CpuJiffies cpu_jiffies;
  cpu_jiffies.user = 3;
  ASSERT_EQ(GetSumJiffies(cpu_jiffies), 3);
}

TEST_F(ProcStatTest, GetProcPidStat) {
  ProcessStat process_stat;
  ASSERT_FALSE(GetProcPidStat(0, &process_stat));
  pid_t pid = getpid();
  ASSERT_TRUE(GetProcPidStat(pid, &process_stat));
}

TEST_F(ProcStatTest, GetSumPidJiffies) {
  ProcessStat process_stat;
  pid_t pid = getpid();
  ASSERT_GE(GetSumPidJiffies(pid), 0);
  process_stat.utime = 3;
  process_stat.stime = 2;
  ASSERT_EQ(GetSumPidJiffies(process_stat), 5);
}

}  // namespace gdt
