// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17
#include <vector>
#include "app/qzap/common/base/base.h"
#include "app/qzap/common/base/shared_ptr.h"
#include "app/qzap/common/base/callback.h"
#include "app/qzap/common/base/thread.h"
#include "app/qzap/common/thread/mutex.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

class MutexTest : public testing::Test {
};

static void Inc(Mutex *mutex, int *i) {
  MutexLock lock(mutex);
  ++(*i);
}

static Mutex global_mutex;
TEST_F(MutexTest, Test1) {
  static const int kTestNumber = 300;
  int j = 0;
  std::vector<shared_ptr<Thread> > threads;
  for (int i = 0; i < kTestNumber; ++i) {
    shared_ptr<Thread> t(new Thread(NewCallback(&Inc, &global_mutex, &j)));
    t->Start();
    threads.push_back(t);
  }
  for (int i = 0; i < kTestNumber; ++i) {
    threads[i]->Join();
  }
  ASSERT_EQ(j, kTestNumber);
}

TEST_F(MutexTest, Test2) {
  static const int kTestNumber = 300;
  int j = 0;
  std::vector<shared_ptr<Thread> > threads;
  Mutex mutex;
  for (int i = 0; i < kTestNumber; ++i) {
    shared_ptr<Thread> t(new Thread(NewCallback(&Inc, &mutex, &j)));
    t->Start();
    threads.push_back(t);
  }
  for (int i = 0; i < kTestNumber; ++i) {
    threads[i]->Join();
  }
  ASSERT_EQ(j, kTestNumber);
}
