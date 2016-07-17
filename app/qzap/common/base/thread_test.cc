// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
#include <string>
#include <vector>
#include "app/qzap/common/base/base.h"
#include "app/qzap/common/base/thread.h"
#include "app/qzap/common/base/shared_ptr.h"
#include "app/qzap/common/base/string_utility.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

class ThreadTest : public testing::Test {
};

static void Assign(int i, int *j) {
  *j = i;
}

static void Inc(volatile int64_t *j) {
  // 3000000000LL loops cost about 7.5s
  for (int64_t i = 0; i < 3000000000LL; ++i) {
    ++(*j);
  }
}

static void GetCpuData(int idx, int64_t *idle, int64_t *total) {
  std::string cpu = StringPrintf("cpu%d", idx);
  FILE *fp = fopen("/proc/stat", "r");
  assert(fp);
  char buf[512] = { 0 };
  while (!feof(fp)) {
    memset(buf, 0x00, sizeof(buf));
    fgets(buf, sizeof(buf)-1, fp);
    if (strncmp(buf, cpu.c_str(), cpu.length()) == 0) {
      break;
    }
  }
  fclose(fp);
  // cpu_i user nice system idle iowait irq softirq xx xx
  std::vector<int64_t> vi;
  char *ptr = NULL;
  char *ptr_tmp = NULL;
  ptr = strtok_r(buf, " ", &ptr_tmp);
  while (ptr) {
    vi.push_back(atol(ptr));
    ptr = strtok_r(NULL, " ", &ptr_tmp);
  }
  ASSERT_GE(static_cast<int>(vi.size()), 8);
  while (vi.size() != 8) vi.pop_back();
  *idle = vi[4];
  std::vector<int64_t>::const_iterator it;
  for (it = vi.begin(); it != vi.end(); ++it) {
    *total += (*it);
  }
}

static void GetCpuRatio(int idx, int span, double* ratio) {
  int64_t idle1 = 0, idle2 = 0;
  int64_t total1 = 0, total2 = 0;
  GetCpuData(idx, &idle1, &total1);
  sleep(span);
  GetCpuData(idx, &idle2, &total2);
  *ratio = 100.0 - (idle2-idle1)*1.0/(total2-total1)*100.0;
  LOG(INFO) << "cpu"<< idx << "-ratio=" << *ratio;
}

TEST_F(ThreadTest, Test1) {
  int i = 1;
  Thread t(NewCallback(&Assign, 0, &i));
  ASSERT_EQ(i, 1);
  t.Start();
  t.Join();
  ASSERT_EQ(i, 0);
}

// Use the main thread to read /proc/stat instead of
// runing the test and using top to check setting affinity.
TEST_F(ThreadTest, Test2) {
  const int kThreadNumber = 2;
  const int kCpuIdx = 0;
  const int kCheckSpan = 5;
  const double kRatioThreshold = 99;  // 99%
  std::vector<shared_ptr<Thread> > threads;
  volatile int64_t j = 0;
  for (int i = 0; i < kThreadNumber; ++i) {
    shared_ptr<Thread> t(new Thread(NewCallback(&Inc, &j)));
    threads.push_back(t);
    t->SetAffinity(kCpuIdx);
    t->Start();
  }
  sleep(1);
  double ratio;
  GetCpuRatio(kCpuIdx, kCheckSpan, &ratio);
  EXPECT_GE(ratio, kRatioThreshold);
  for (int i = 0; i < kThreadNumber; ++i) {
    threads[i]->Join();
  }
}
