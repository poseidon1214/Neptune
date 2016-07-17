// Copyright (c) 2012, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2012-06-21

#include "common/system/concurrency/this_thread.h"
#include "common/base/annotation.h"
#include "common/system/concurrency/thread.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

static void IsMainThreadTestThread(bool* b) {
  *b = ThisThread::IsMain();
}

TEST(ThisThread, GetId) {
  EXPECT_GT(ThisThread::GetId(), 0);
}

TEST(ThisThread, GetIdBench) {
  int total = 0;
  for (int i = 0; i < 100000000; ++i)
    total += ThisThread::GetId();
  volatile int other_total = total;
  IgnoreUnused(other_total);
}

TEST(ThisThread, GetHandle) {
#if defined _WIN32 || defined __linux__
  ThreadHandleType null_id = 0;
  EXPECT_NE(null_id, ThisThread::GetHandle());
#endif
}

TEST(ThisThread, Exit) {
  // EXPECT_EXIT(ThisThread::Exit(), ::testing::ExitedWithCode(0), "");
}

TEST(ThisThread, IsMainThread) {
  EXPECT_TRUE(ThisThread::IsMain());
  bool is_main = true;
  Thread thread(NewCallback(&IsMainThreadTestThread, &is_main));
  EXPECT_TRUE(thread.Start());
  thread.Join();
  EXPECT_FALSE(is_main);
}

TEST(ThisThread, Yield) {
  ThisThread::Yield();
}

}  // namespace gdt
