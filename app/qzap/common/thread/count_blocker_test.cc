// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
#include <sstream>
#include <vector>
#include "third_party/gflags/gflags.h"
#include "third_party/glog/logging.h"
#include "third_party/gtest/gtest.h"
#include "thread/count_blocker.h"
#include "thread/threadpool.h"

class CountBlockerTest : public testing::Test {
 public:
  void Wait(Callback<bool()> *h,
           int *cnt) {
    VLOG(2) << "Inc call, cnt: " << *cnt << " wait";
    if (!h->Run()) {
      return;
    }
    VLOG(2) << "Inc call, cnt: " << *cnt;
    *cnt = 0xbeef;
  }
 protected:
  static const int kPoolSize = 100;
  static const int kItemSize = 10000;
};

static bool WaitForCounter(shared_ptr<CountBlocker> counter, int max) {
  return counter->Wait(max);
}

TEST_F(CountBlockerTest, TestWait) {
  static const int kLoop = 100;
  shared_ptr<ThreadPool> p1(ThreadPool::Create("TestWait", kPoolSize));
  shared_ptr<ThreadPool> p2(ThreadPool::Create("TestCountBlocker", kPoolSize));
  vector<shared_ptr<CountBlocker> > ns;
  vector<int> v;
  v.resize(kItemSize, 0);
  p1->Start();
  for (int k = 0; k < kItemSize; ++k) {
    shared_ptr<CountBlocker> n(new CountBlocker(kLoop));
    Callback<bool()> *h = NewCallback(WaitForCounter, n, 1000000);
    p1->PushTask(NewCallback(this, &CountBlockerTest::Wait, h, &v[k]));
    ns.push_back(n);
  }
  for (int i = 0; i < kItemSize; ++i) {
    EXPECT_EQ(v[i], 0);
  }
  p2->Start();
  VLOG(0) << "p2 start";
  for (int j = 0; j < kLoop; ++j) {
    for (int k = 0; k < kItemSize; ++k) {
      p2->PushTask(NewCallback(ns[k],
          &CountBlocker::Dec, static_cast<int64>(1ll)));
    }
  }
  p2->Stop();
  p1->Stop();
  for (int i = 0; i < kItemSize; ++i) {
    EXPECT_EQ(v[i], 0xbeef) << i;
  }
}

TEST_F(CountBlockerTest, TestWaitOut) {
  shared_ptr<ThreadPool> p1(ThreadPool::Create("TestCountBlocker", kPoolSize));
  vector<shared_ptr<CountBlocker> > ns;
  vector<int> v;
  v.resize(kItemSize, 0);
  p1->Start();
  for (int k = 0; k < kItemSize; ++k) {
    shared_ptr<CountBlocker> n(new CountBlocker(kItemSize - 1));
    Callback<bool()> *h = NewCallback(WaitForCounter, n, 1);
    p1->PushTask(
        NewCallback(this, &CountBlockerTest::Wait, h, &v[k]));
    ns.push_back(n);
  }
  p1->Stop();
  for (int i = 0; i < kItemSize; ++i) {
    EXPECT_EQ(v[i], 0);
  }
}

int main(int argc, char **argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
