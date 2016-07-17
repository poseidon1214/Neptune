// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/13/11
// Description:

#include "common/system/concurrency/thread.h"
#include "common/system/concurrency/this_thread.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

static void ThreadCallback(int* p) {
  ++*p;
}

TEST(Thread, Test) {
  int n = 0;
  Thread thread(NewCallback(ThreadCallback, &n));
  thread.Start();
  thread.Join();
  EXPECT_EQ(1, n);
}

static void DoNothing() {
}

TEST(Thread, Restart) {
  Thread thread(NewPermanentCallback(DoNothing));
  for (int i = 0; i < 10; ++i) {
    thread.Start();
    int tid1 = thread.Id();
    thread.Join();

    thread.Start();
    int tid2 = thread.Id();
    EXPECT_NE(tid1, tid2);
    thread.Join();
  }
}

TEST(Thread, Reinitialize) {
  Thread thread(NewPermanentCallback(DoNothing));
  thread.Start();
  thread.Join();
  thread.Start();
  thread.Join();

  thread.Initialize(NewPermanentCallback(DoNothing));
  thread.Start();
  thread.Join();
  thread.Start();
  thread.Join();
}

TEST(Thread, RestartDeathTest) {
  testing::FLAGS_gtest_death_test_style = "threadsafe";
  Thread thread;
  thread.Initialize(NewCallback(DoNothing));
  thread.Start();
  thread.Join();
  EXPECT_DEATH(thread.Start(), "Invalid argument");
}

TEST(Thread, DuplicatedStartDeathTest) {
  testing::FLAGS_gtest_death_test_style = "threadsafe";
  {
    Thread thread(NewPermanentCallback(DoNothing));
    thread.Start();
    EXPECT_DEATH(thread.Start(), "Invalid argument");
    thread.Join();
  }
}

static void IsAliveTestThread(volatile const bool* stop) {
  while (!*stop)
    ThisThread::Sleep(1);
  // ThisThread::Exit();
}

TEST(Thread, IsAlive) {
  bool stop = false;
  Thread thread(NewCallback(IsAliveTestThread, &stop));
  thread.Start();
  for (int i = 0; i < 1000; ++i) {
    if (!thread.IsAlive())
      ThisThread::Sleep(1);
  }
  stop = true;
  thread.Join();
  EXPECT_FALSE(thread.IsAlive());
}

TEST(Thread, Detach) {
  for (int i = 0; i < 100; ++i) {
    Thread thread(NewCallback(ThisThread::Sleep, 1));
    thread.Start();
    thread.Detach();
    ThisThread::Sleep(1);
  }
}

TEST(Thread, CreateDetach) {
  ThreadAttributes attributes;
  attributes.SetDetached(true);
  for (int i = 0; i < 100; ++i) {
    Thread thread(attributes, NewCallback(ThisThread::Sleep, 1));
    thread.Start();
    ASSERT_FALSE(thread.IsJoinable());
  }
}

}  // namespace gdt
