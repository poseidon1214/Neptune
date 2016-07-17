// Copyright (c) 2010, Tencent Inc. All rights reserved.

#include <iostream>
#include "common/base/callback.h"
#include "common/system/concurrency/thread.h"
#include "common/system/timer/timer_manager.h"
#include "thirdparty/gtest/gtest.h"

using std::cout;
using std::endl;

namespace gdt {

static int n1 = 0;
static int n2 = 0;
static int n3 = 0;
static int n4 = 0;
static int n5 = 0;

class TimerManagerTest : public testing::Test {
 public:
  void EchoCallBack(int *count, uint64_t timer_id) {
    (*count)++;
    cout << "echo callback, timer: " <<
        timer_id << ", count: " << *count << endl;
  }

  void RemoveSelf(int *count, uint64_t timer_id) {
    (*count)++;
    cout << "remove self, timer: " << timer_id
        << ", count: " << *count << endl;
    timer_manager.RemoveTimer(timer_id);
  }

  void GetTimerManagerInfo() {
    TimerManager::Stats info;
    timer_manager.GetStats(&info);
    cout << "--------------------\n";
    cout << "oneshot: " << info.oneshot_timer_num << endl;
    cout << "period:  " << info.period_timer_num << endl;
    cout << "runover: " << info.estimate_runover_time << endl;
    cout << "--------------------\n";
  }

 protected:
    TimerManager timer_manager;
};

TEST_F(TimerManagerTest, EchoTimerTest) {
  // 添加一个周期性定时器
  TimerManager::CallbackClosure *cb1 = NewPermanentCallback(
      this, &TimerManagerTest::EchoCallBack, &n1);
  uint64_t id1 = timer_manager.AddPeriodTimer(500, cb1);

  // 添加一个短期触发的一次性定时器
  TimerManager::CallbackClosure *cb2 = NewCallback(
      this, &TimerManagerTest::EchoCallBack, &n2);
  timer_manager.AddOneshotTimer(14, cb2);

  // 添加一个短时间触发的周期性定时器
  TimerManager::CallbackClosure *cb3 = NewPermanentCallback(
      this, &TimerManagerTest::EchoCallBack, &n3);
  uint64_t id3 = timer_manager.AddPeriodTimer(20, cb3);

  // 添加一个不可能触发的一次性定时器
  TimerManager::CallbackClosure *cb4 = NewCallback(
      this, &TimerManagerTest::EchoCallBack, &n4);
  uint64_t id4 = timer_manager.AddOneshotTimer(1000000000000LL, cb4);

  // 休息一秒，等待一次性定时器执行完毕
  ThisThread::Sleep(1000);
  // 查看此时定时器管理类中的统计信息
  GetTimerManagerInfo();

  // 修改周期定时器的属性，增长触发时间
  timer_manager.ModifyTimer(id3, 200);
  ThisThread::Sleep(1000);

  // 修改周期性定时器，在回调中自删除
  cout << "timer 3 will be modified to self delete:\n";
  cb3 = NewPermanentCallback(this, &TimerManagerTest::RemoveSelf, &n3);
  timer_manager.ModifyTimer(id3, 200, cb3);

  // 修改一次性定时器的属性，并在回调中删除自己
  cout << "timer 4 will be modified to self delete:\n";
  cb4 = NewCallback(this, &TimerManagerTest::RemoveSelf, &n4);
  timer_manager.ModifyTimer(id4, 1000, cb4);

  // 等待上述两个定时器执行完毕
  ThisThread::Sleep(1000);
  GetTimerManagerInfo();

  // 再添加一个不可能触发的一次性定时器
  TimerManager::CallbackClosure *cb5 =
    NewCallback(this, &TimerManagerTest::EchoCallBack, &n5);
  timer_manager.AddOneshotTimer(100000000LL, cb5);

  timer_manager.RemoveTimer(id1);
  GetTimerManagerInfo();
}

void Inc(int* p, uint64_t id) {
  ++*p;
}

TEST_F(TimerManagerTest, Performance) {
  int n = 0;
  for (int i = 0; i < 500; ++i) {
    for (int j = 0; j < 1000; ++j) {
      timer_manager.AddOneshotTimer(i, NewCallback(&Inc, &n));
    }
  }

  std::cout << "n = " << n << '\n';
  ThisThread::Sleep(1000);
  std::cout << "n = " << n << '\n';
}

TEST(TimerManager, Stable) {
  for (int i = 0; i < 1000; ++i) {
    TimerManager timer_manager;
  }
}

TEST(TimerManager, DefaultInstance) {
  TimerManager& timer_manager = TimerManager::DefaultInstance();
  (void) timer_manager;
}

void OrderTest(int *n, uint64_t id) {
  ++*n;
}

TEST(TimerManager, Order) {
  TimerManager& timer_manager = TimerManager::DefaultInstance();
  int n = 0;
  uint64_t id = timer_manager.AddOneshotTimer(
      10000, NewCallback(&OrderTest, &n));
  timer_manager.AddOneshotTimer(0, NewCallback(&OrderTest, &n));
  ThisThread::Sleep(100);
  timer_manager.RemoveTimer(id);

  EXPECT_EQ(1, n);
}

static void DoNothing(uint64_t id) {
}

TEST(TimerManager, Stop) {
  TimerManager timer_manager;
  timer_manager.AddOneshotTimer(10, NewCallback(&DoNothing));
  timer_manager.AddPeriodTimer(100, NewPermanentCallback(&DoNothing));
  timer_manager.Stop();
  timer_manager.Stop();
}

}  // namespace gdt
