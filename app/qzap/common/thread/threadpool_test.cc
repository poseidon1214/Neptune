// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17
#include <inttypes.h>
#include <sys/sysinfo.h>

#include <sstream>
#include <vector>

#include "app/qzap/common/base/sysinfo.h"
#include "app/qzap/common/thread/threadpool.h"
#include "app/qzap/common/utility/time_utility.h"

#include "thirdparty/gtest/gtest.h"

class ThreadPoolTest : public testing::Test {
 public:
  void Inc(int *cnt) {
    *cnt = 0xbeef;
  }
 protected:
  static const int kPoolSize = 100;
};

TEST_F(ThreadPoolTest, Test1) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("Test", kPoolSize));
  srand(time(NULL));
  uint32_t seed = time(NULL) % 1000;
  for (int k = 0; k < 1000; ++k) {
    p->Start();
    int item_size = kPoolSize * (rand_r(&seed) % 10 + 1);
    LOG(INFO) << "item size: " << item_size;
    std::vector<int> v;
    v.resize(item_size, 0);
    for (int i = 0; i < item_size; ++i) {
      Closure *handler = NewCallback(this, &ThreadPoolTest::Inc, &v[i]);
      p->PushTask(handler);
    }
    p->Stop();
    for (int i = 0; i < item_size; ++i) {
      EXPECT_EQ(v[i], 0xbeef);
    }
    LOG(INFO) << "thread stopped" << k;
  }
}


TEST_F(ThreadPoolTest, TestBindCpu) {
  std::vector<int> cpu;
  int total_cpu_core_num = get_nprocs();
  if (total_cpu_core_num < 3) {
    return;
  }
  cpu.push_back(1 % total_cpu_core_num);
  cpu.push_back(2 % total_cpu_core_num);
  shared_ptr<ThreadPool> p(ThreadPool::CreatePool("TestCpu", kPoolSize, cpu));
  uint32_t seed = time(NULL) % 1000;
  for (int k = 0; k < 1000; ++k) {
    p->Start();
    int item_size = kPoolSize * (rand_r(&seed) % 10 + 1);
    LOG(INFO) << "item size: " << item_size;
    std::vector<int> v;
    v.resize(item_size, 0);
    for (int i = 0; i < item_size; ++i) {
      Closure *handler = NewCallback(this, &ThreadPoolTest::Inc, &v[i]);
      p->PushTask(handler);
    }
    p->Stop();
    for (int i = 0; i < item_size; ++i) {
      EXPECT_EQ(v[i], 0xbeef);
    }
    LOG(INFO) << "thread stopped" << k;
  }
}

// Test cpu affinity
static void Inc(volatile int64_t *j) {
  // 3000000000LL loops cost about 7.5s
  for (int64_t i = 0; i < 3000000000LL; ++i) {
    ++(*j);
  }
}

TEST_F(ThreadPoolTest, TestBindCpu2) {
  const int kCpuIdx = 1;
  const int kCheckSpan = 5;
  const double kRatioThreshold = 99;  // 99%
  std::vector<int> cpu;
  int total_cpu_core_num = get_nprocs();
  if (total_cpu_core_num < 2) {
    return;
  }
  cpu.push_back(1 % total_cpu_core_num);
  shared_ptr<ThreadPool> p(ThreadPool::CreatePool("TestCpu2", 2, cpu));
  p->Start();
  volatile int64_t j = 0;
  p->PushTask(NewCallback(&::Inc, &j));
  SleepForSeconds(1);
  double ratio;
  GetCpuRatio(kCpuIdx, kCheckSpan, &ratio);
  EXPECT_GE(ratio, kRatioThreshold);
}
