// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17
#include <stdlib.h>
#include <sstream>
#include <vector>
#include "app/qzap/common/base/benchmark.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/sysinfo.h"
#include "app/qzap/common/thread/threadworker.h"
#include "app/qzap/common/base/walltime.h"
#include "app/qzap/common/utility/time_utility.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/gtest/gtest.h"
#include "app/qzap/common/thread/count_blocker.h"
#include "app/qzap/common/base/string_utility.h"
class ThreadWorkerTest : public testing::Test {
 public:
  void Inc(int *cnt) {
    VLOG(2) << "Inc call, kTestSize " << *cnt;
    *cnt = 0xbeef;
  }

  void Check(int i) {
    ASSERT_EQ(i_ + 1, i);
    i_ = i;
  }

  void GetThreadId(CountBlocker *counter, int32_t *tid) {
    *tid = static_cast<int32_t>(pthread_self());
    counter->Dec(1);
  }

 protected:
  int i_;
  static const int kPoolSize = 100;
  static const int kTestSize = 1000;
};

TEST_F(ThreadWorkerTest, Test1) {
  shared_ptr<ThreadWorker> p(ThreadWorker::Create("Test"));
  srand(time(NULL));
  for (int k = 0; k < kTestSize; ++k) {
    p->Start();
    int item_size = kPoolSize * (rand() % 10 + 1);
    VLOG(2) << "item size: " << item_size;
    std::vector<int> v;
    v.resize(item_size, 0);
    for (int i = 0; i < item_size; ++i) {
      Closure* handler = NewCallback(this, &ThreadWorkerTest::Inc, &v[i]);
      if (!p->PushTask(handler)) {
        delete handler;
      }
    }
    VLOG(2) << "Begin to stop";
    p->Stop();
    for (int i = 0; i < item_size; ++i) {
      EXPECT_EQ(v[i], 0xbeef) << i;
    }
    VLOG(0) << "thread stopped" << k;
  }
}

TEST_F(ThreadWorkerTest, Test2) {
  shared_ptr<ThreadWorker> a(ThreadWorker::Create("Test"));
  weak_ptr<ThreadWorker> b(a);
  shared_ptr<ThreadWorker> p(b.lock());
  srand(time(NULL));
  for (int k = 0; k < kTestSize; ++k) {
    p->Start();
    int item_size = kPoolSize * (rand() % 10 + 1);
    VLOG(2) << "item size: " << item_size;
    std::vector<int> v;
    v.resize(item_size, 0);
    for (int i = 0; i < item_size; ++i) {
      Closure* handler = NewCallback(this, &ThreadWorkerTest::Inc, &v[i]);
      if (!p->PushTask(handler)) {
        delete handler;
      }
    }
    p->Stop();
    for (int i = 0; i < item_size; ++i) {
      EXPECT_EQ(v[i], 0xbeef) << i;
    }
    VLOG(0) << "thread stopped" << k;
  }
}

TEST_F(ThreadWorkerTest, TestSequence) {
  shared_ptr<ThreadWorker> p(ThreadWorker::Create("Test"));
  srand(time(NULL));
  for (int k = 0; k < kTestSize; ++k) {
    p->Start();
    int item_size = kPoolSize * (rand() % 10 + 1);
    VLOG(2) << "item size: " << item_size;
    i_ = -1;
    for (int i = 0; i < item_size; ++i) {
      Closure* handler = NewCallback(this, &ThreadWorkerTest::Check, i);
      p->PushTask(handler);
    }
    p->Stop();
    VLOG(0) << "thread stopped" << k;
  }
}

void Expired(CountBlocker *counter, bool *expired) {
  *expired = true;
  if (counter != NULL) {
    counter->Dec(1);
  }
}

// Test cpu affinity
static void Inc(volatile int64_t *j) {
  // 3000000000LL loops cost about 7.5s
  for (int64_t i = 0; i < 3000000000LL; ++i) {
    ++(*j);
  }
}

// Use the main thread to read /proc/stat instead of
// runing the test and using top to check setting affinity.
TEST_F(ThreadWorkerTest, Test8) {
  const int kThreadNumber = 2;
  const int kCpuIdx = 0;
  const int kCheckSpan = 5;
  const double kRatioThreshold = 99;  // 99%
  std::vector<shared_ptr<ThreadWorker> > threads;
  volatile int64_t j = 0;
  for (int i = 0; i < kThreadNumber; ++i) {
    shared_ptr<ThreadWorker> p(ThreadWorker::Create("Test"));
    p->set_cpu(kCpuIdx);
    p->Start();
    p->PushTask(NewCallback(&::Inc, &j));
    threads.push_back(p);
  }
  SleepForSeconds(1);
  double ratio;
  GetCpuRatio(kCpuIdx, kCheckSpan, &ratio);
  EXPECT_GE(ratio, kRatioThreshold);
}

class BenchMarkPushTaskTest {
 public:
  BenchMarkPushTaskTest() {
    thread_worker_ = ThreadWorker::Create("BenchmarkTest");
    thread_worker_->Start();
  }

  ~BenchMarkPushTaskTest() {
    thread_worker_->Stop();
  }

  void PushTask(Closure*c) {
    thread_worker_->PushTask(c);
  }

  void CallBack() {
  }

 private:
  shared_ptr<ThreadWorker> thread_worker_;
};
static BenchMarkPushTaskTest benchmark_pushtask;
static void BenchMarkTestPushTask(int n) {
  for (int i = 0; i < n; ++i) {
    benchmark_pushtask.PushTask(NewCallback(&benchmark_pushtask,
        &BenchMarkPushTaskTest::CallBack));
  }
}
BENCHMARK(BenchMarkTestPushTask);

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  RunSpecifiedBenchmarks();
  return RUN_ALL_TESTS();
}
