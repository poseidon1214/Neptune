// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2010-06-18

#include "common/system/concurrency/event.h"

#include <stdint.h>
#include <iostream>  // NOLINT

#include "common/system/concurrency/blocking_queue.h"
#include "common/system/concurrency/this_thread.h"
#include "app/qzap/common/base/thread.h"
#include "common/system/time/clock.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

TEST(AutoResetEvent, SetAndWait) {
  AutoResetEvent event;
  EXPECT_FALSE(event.TryWait());
  event.Set();
  EXPECT_TRUE(event.TryWait());
  EXPECT_FALSE(event.TryWait());
}

TEST(AutoResetEvent, TimedWait) {
  AutoResetEvent event;
  EXPECT_FALSE(event.TimedWait(1));
  event.Set();
  EXPECT_TRUE(event.TimedWait(1));
  EXPECT_FALSE(event.TryWait());
}


TEST(AutoResetEvent, InitValue) {
  AutoResetEvent event(true);
  EXPECT_TRUE(event.TryWait());
  EXPECT_FALSE(event.TryWait());
  event.Set();
  EXPECT_TRUE(event.TryWait());
}

TEST(AutoResetEvent, Set) {
  AutoResetEvent event(false);
  event.Set();
  EXPECT_TRUE(event.TryWait());
}

TEST(AutoResetEvent, Reset) {
  AutoResetEvent event(true);
  event.Reset();
  EXPECT_FALSE(event.TryWait());
}

static void WakeThread(BlockingQueue<AutoResetEvent*>* queue) {
  for (;;) {
    AutoResetEvent* e;
    queue->PopFront(&e);
    if (!e)
      break;
    e->Set();
  }
}

TEST(AutoResetEvent, DestroyAfterSet) {
  BlockingQueue<AutoResetEvent*> queue;
  Thread thread(NewCallback(WakeThread, &queue));
  thread.Start();

  for (int i = 0; i < 1000; ++i) {
    if (i % 64 == 0)
      VLOG(3) << "Wait Time: " << i;
    AutoResetEvent* e;
    {
      AutoResetEvent event;
      e = &event;
      queue.PushBack(&event);
      event.Wait();
    }
    memset(e, 0xFF, sizeof(*e));
    ThisThread::Sleep(1);
  }

  queue.PushBack(NULL);
  thread.Join();
}

TEST(ManualResetEvent, SetAndWait) {
  ManualResetEvent event;
  EXPECT_FALSE(event.TryWait());
  event.Set();
  EXPECT_TRUE(event.TryWait());
  EXPECT_TRUE(event.TryWait());
  event.Reset();
  EXPECT_FALSE(event.TryWait());
}

TEST(ManualResetEvent, InitValue) {
  ManualResetEvent event(true);
  EXPECT_TRUE(event.TryWait());
  EXPECT_TRUE(event.TryWait());
  event.Set();
  EXPECT_TRUE(event.TryWait());
  EXPECT_TRUE(event.TryWait());
}

TEST(ManualResetEvent, TimedWait) {
  ManualResetEvent event(false);
  event.Set();
  EXPECT_TRUE(event.TimedWait(1));
  event.Reset();
  EXPECT_FALSE(event.TimedWait(1));
}


TEST(ManualResetEvent, Set) {
  ManualResetEvent event(false);
  event.Set();
  event.Wait();
  EXPECT_TRUE(event.TryWait());
}

TEST(ManualResetEvent, Reset) {
  ManualResetEvent event(true);
  event.Reset();
  EXPECT_FALSE(event.TryWait());
}

}  // namespace gdt
