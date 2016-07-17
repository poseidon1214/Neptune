// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17
#include <vector>
#include "app/qzap/common/base/base.h"
#include "app/qzap/common/base/walltime.h"
#include "app/qzap/common/base/shared_ptr.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/callback.h"
#include "app/qzap/common/base/thread.h"
#include "app/qzap/common/thread/mutex.h"
#include "app/qzap/common/thread/shared_mutex.h"
#include "app/qzap/common/thread/threadpool.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"


#define CHECK_LOCKED_VALUE_EQUAL(mutex_name, value, expected_value)    \
    {                                                                \
        MutexLock lock(&mutex_name);                  \
        EXPECT_EQ(value, expected_value);                     \
    }

template <class LockType>
class LockingThread {
 private:
  SharedMutex *rw_mutex_;
  unsigned *unblocked_count_;
  CondVar *unblocked_condition_;
  unsigned *simultaneous_running_count_;
  unsigned *max_simultaneous_running_;
  Mutex *unblocked_count_mutex_;
  Mutex *finish_mutex_;

 public:
  LockingThread(SharedMutex *rw_mutex,
                unsigned *unblocked_count,
                Mutex *unblocked_count_mutex,
                CondVar *unblocked_condition,
                Mutex *finish_mutex,
                unsigned *simultaneous_running_count,
                unsigned *max_simultaneous_running):
    rw_mutex_(rw_mutex),
    unblocked_count_(unblocked_count),
    unblocked_condition_(unblocked_condition),
    simultaneous_running_count_(simultaneous_running_count),
    max_simultaneous_running_(max_simultaneous_running),
    unblocked_count_mutex_(unblocked_count_mutex),
    finish_mutex_(finish_mutex) {
  }

  void operator()() {
    // acquire lock
    LOG(INFO) << "LockingThread start to run";
    LockType lock(rw_mutex_);
    // increment count to show we're unblocked
    {
      LOG(INFO) << "LockingThread before ublock";
      MutexLock ublock(unblocked_count_mutex_);
      ++*unblocked_count_;
      LOG(INFO) << "LockingThread after ublock:" << *unblocked_count_;
      unblocked_condition_->Notify();
      ++*simultaneous_running_count_;
      if (*simultaneous_running_count_ > *max_simultaneous_running_) {
        *max_simultaneous_running_ = *simultaneous_running_count_;
      }
    }

    LOG(INFO) << "LockingThread wait to finish";
    // wait to finish
    MutexLock finish_lock(finish_mutex_);
    {
      MutexLock ublock(unblocked_count_mutex_);
      --*simultaneous_running_count_;
    }
    delete this;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(LockingThread);
};

typedef LockingThread<SharedMutex::ScopedLock> TestWriterThread;
typedef LockingThread<SharedMutex::ScopedSharedLock> TestReaderThread;
typedef LockingThread<SharedMutex::ScopedUpgradeLock> TestUpgradeThread;

class SimpleWritingThread {
 public:
  SimpleWritingThread(SharedMutex *rwm,
                      Mutex *finish_mutex,
                      Mutex *unblocked_mutex,
                      unsigned *unblocked_count) :
    rwm_(rwm), finish_mutex_(finish_mutex),
    unblocked_mutex_(unblocked_mutex), unblocked_count_(unblocked_count) {
    }
  void operator()() {
    SharedMutex::ScopedLock lk(rwm_);
    {
      MutexLock ulk(unblocked_mutex_);
      ++*unblocked_count_;
    }
    MutexLock flk(finish_mutex_);
    delete this;
  }

 private:
  SharedMutex *rwm_;
  Mutex *finish_mutex_;
  Mutex *unblocked_mutex_;
  unsigned *unblocked_count_;
  DISALLOW_COPY_AND_ASSIGN(SimpleWritingThread);
};

class SimpleReadingThread {
 public:
  SimpleReadingThread(SharedMutex *rwm,
                      Mutex *finish_mutex,
                      Mutex *unblocked_mutex,
                      unsigned *unblocked_count)
    : rwm_(rwm), finish_mutex_(finish_mutex),
    unblocked_mutex_(unblocked_mutex),
    unblocked_count_(unblocked_count) {
    }

  void operator()() {
    SharedMutex::ScopedSharedLock lk(rwm_);
    {
      MutexLock ulk(unblocked_mutex_);
      ++*unblocked_count_;
    }
    MutexLock flk(finish_mutex_);
    delete this;
  }

 private:
  SharedMutex *rwm_;
  Mutex *finish_mutex_;
  Mutex *unblocked_mutex_;
  unsigned *unblocked_count_;
  DISALLOW_COPY_AND_ASSIGN(SimpleReadingThread);
};

class SimpleUpgradeThread {
 public:
  SimpleUpgradeThread(
      SharedMutex *rwm,
      Mutex *finish_mutex,
      Mutex *unblocked_mutex,
      unsigned *unblocked_count)
    : rwm_(rwm), finish_mutex_(finish_mutex),
      unblocked_mutex_(unblocked_mutex),
      unblocked_count_(unblocked_count) {
  }

  void operator()() {
    SharedMutex::ScopedUpgradeLock lk(rwm_);
    {
      MutexLock ulk(unblocked_mutex_);
      ++*unblocked_count_;
    }
    MutexLock flk(finish_mutex_);
    delete this;
  }

 private:
  SharedMutex *rwm_;
  Mutex *finish_mutex_;
  Mutex *unblocked_mutex_;
  unsigned *unblocked_count_;
  DISALLOW_COPY_AND_ASSIGN(SimpleUpgradeThread);
};

class SharedMutexTest : public testing::Test {
};

static void Inc(SharedMutex *mutex, int *i) {
  mutex->Lock();
  ++(*i);
  mutex->Unlock();
}

static SharedMutex global_mutex;
TEST_F(SharedMutexTest, Test1) {
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

TEST_F(SharedMutexTest, Test2) {
  static const int kTestNumber = 300;
  int j = 0;
  std::vector<shared_ptr<Thread> > threads;
  SharedMutex mutex;
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

TEST_F(SharedMutexTest, TestMultipleReaders) {
  typedef LockingThread<SharedMutex::ScopedSharedLock> TestThread;
  unsigned const number_of_threads = 10;

  shared_ptr<ThreadPool> pool(
      ThreadPool::Create("TestMultipleReaders", number_of_threads));
  pool->Start();
  SharedMutex rw_mutex;
  unsigned unblocked_count = 0;
  unsigned simultaneous_running_count = 0;
  unsigned max_simultaneous_running = 0;
  Mutex unblocked_count_mutex;
  CondVar unblocked_condition;
  Mutex finish_mutex;
  {
    MutexLock finish_lock(&finish_mutex);

    for (unsigned i = 0; i < number_of_threads; ++i) {
      TestThread *t(
          new TestThread(
              &rw_mutex, &unblocked_count, &unblocked_count_mutex,
              &unblocked_condition,
              &finish_mutex,
              &simultaneous_running_count,
              &max_simultaneous_running));
      pool->PushTask(NewCallback(
          t, &TestThread::operator()));
    }
    {
      MutexLock lk(&unblocked_count_mutex);
      while (unblocked_count < number_of_threads) {
        unblocked_condition.Wait(&unblocked_count_mutex);
      }
    }

    CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
                             unblocked_count,
                             number_of_threads);
  }
  pool->Stop();
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
                           max_simultaneous_running,
                           number_of_threads);
}

TEST_F(SharedMutexTest, TestOnlyOneWriterPermitted) {
  typedef LockingThread<SharedMutex::ScopedLock> TestThread;
  unsigned const number_of_threads = 10;

  shared_ptr<ThreadPool> pool(
      ThreadPool::Create("TestOnlyOneWriterPermitted", number_of_threads));
  pool->Start();
  SharedMutex rw_mutex;
  unsigned unblocked_count = 0;
  unsigned simultaneous_running_count = 0;
  unsigned max_simultaneous_running = 0;
  Mutex unblocked_count_mutex;
  CondVar unblocked_condition;
  Mutex finish_mutex;
  {
    MutexLock finish_lock(&finish_mutex);
    for (unsigned i = 0; i < number_of_threads; ++i) {
      TestThread *t(
          new TestThread(
              &rw_mutex, &unblocked_count, &unblocked_count_mutex,
              &unblocked_condition,
              &finish_mutex,
              &simultaneous_running_count,
              &max_simultaneous_running));
      pool->PushTask(NewCallback(
          t, &TestThread::operator()));
    }
    sleep(2);
    CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex, unblocked_count, 1U);
  }
  pool->Stop();

  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      unblocked_count, number_of_threads);
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      max_simultaneous_running, 1u);
}

TEST_F(SharedMutexTest, TestReaderBlocksWriter) {
  typedef LockingThread<SharedMutex::ScopedLock> TestWriterThread;
  typedef LockingThread<SharedMutex::ScopedSharedLock> TestReaderThread;
  unsigned const number_of_threads = 10;

  shared_ptr<ThreadPool> pool(
      ThreadPool::Create("TestReaderBlocksWriter", number_of_threads));
  pool->Start();
  SharedMutex rw_mutex;
  unsigned unblocked_count = 0;
  unsigned simultaneous_running_count = 0;
  unsigned max_simultaneous_running = 0;
  Mutex unblocked_count_mutex;
  CondVar unblocked_condition;
  Mutex finish_mutex;
  {
    MutexLock finish_lock(&finish_mutex);
    TestReaderThread *t(
        new TestReaderThread(
            &rw_mutex, &unblocked_count, &unblocked_count_mutex,
            &unblocked_condition,
            &finish_mutex,
            &simultaneous_running_count,
            &max_simultaneous_running));
    pool->PushTask(NewCallback(
        t, &TestReaderThread::operator()));

    {
      MutexLock lk(&unblocked_count_mutex);
      while (unblocked_count < 1) {
        unblocked_condition.Wait(&unblocked_count_mutex);
      }
    }
    CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex, unblocked_count, 1U);
    TestWriterThread *t2(
        new TestWriterThread(
            &rw_mutex, &unblocked_count, &unblocked_count_mutex,
            &unblocked_condition,
            &finish_mutex,
            &simultaneous_running_count,
            &max_simultaneous_running));
    pool->PushTask(NewCallback(
        t2, &TestWriterThread::operator()));
    sleep(1);

    CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex, unblocked_count, 1U);
  }
  pool->Stop();
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex, unblocked_count, 2U);
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex, max_simultaneous_running, 1u);
}

TEST_F(SharedMutexTest, TestUnlockingWriterUnblocksAllReaders) {
  typedef LockingThread<SharedMutex::ScopedLock> TestWriterThread;
  typedef LockingThread<SharedMutex::ScopedSharedLock> TestReaderThread;
  unsigned const number_of_threads = 10;
  unsigned const reader_count = 10;

  shared_ptr<ThreadPool> pool(
      ThreadPool::Create(
          "TestUnlockingWriterUnblocksAllReaders", number_of_threads));
  pool->Start();
  SharedMutex rw_mutex;
  unsigned unblocked_count = 0;
  unsigned simultaneous_running_count = 0;
  unsigned max_simultaneous_running = 0;
  Mutex unblocked_count_mutex;
  CondVar unblocked_condition;
  Mutex finish_mutex;
  {
    MutexLock finish_lock(&finish_mutex);
    {
      SharedMutex::ScopedLock write_lock(&rw_mutex);

      for (unsigned i = 0; i < reader_count; ++i) {
        TestReaderThread *t(
            new TestReaderThread(
                &rw_mutex, &unblocked_count, &unblocked_count_mutex,
                &unblocked_condition,
                &finish_mutex,
                &simultaneous_running_count,
                &max_simultaneous_running));
        pool->PushTask(NewCallback(
            t, &TestReaderThread::operator()));
      }
      sleep(1);
      CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex, unblocked_count, 0U);
    }

    {
      MutexLock lk(&unblocked_count_mutex);
      while (unblocked_count < reader_count) {
        unblocked_condition.Wait(&unblocked_count_mutex);
      }
    }

    CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
        unblocked_count, reader_count);
  }
  pool->Stop();
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      max_simultaneous_running, reader_count);
}

TEST_F(SharedMutexTest, TestUnlockingLastReaderOnlyUnblocksOneWriter) {
  unsigned const number_of_threads = 10;
  unsigned const reader_count = 10;
  unsigned const writer_count = 10;

  shared_ptr<ThreadPool> pool(ThreadPool::Create(
      "TestUnlockingLastReaderOnlyUnblocksOneWriter",
      number_of_threads));
  pool->Start();

  SharedMutex rw_mutex;
  unsigned unblocked_count = 0;
  unsigned simultaneous_running_readers = 0;
  unsigned max_simultaneous_readers = 0;
  unsigned simultaneous_running_writers = 0;
  unsigned max_simultaneous_writers = 0;
  Mutex unblocked_count_mutex;
  CondVar unblocked_condition;
  Mutex finish_reading_mutex;
  scoped_ptr<MutexLock> finish_reading_lock(
      new MutexLock(&finish_reading_mutex));

  Mutex finish_writing_mutex;
  scoped_ptr<MutexLock> finish_writing_lock(
      new MutexLock(&finish_writing_mutex));
  for (unsigned i = 0; i < reader_count; ++i) {
    TestReaderThread *t(
        new TestReaderThread(
            &rw_mutex, &unblocked_count, &unblocked_count_mutex,
            &unblocked_condition,
            &finish_reading_mutex,
            &simultaneous_running_readers,
            &max_simultaneous_readers));
    pool->PushTask(NewCallback(
        t, &TestReaderThread::operator()));
  }
  sleep(1);
  for (unsigned i = 0; i < writer_count; ++i) {
    TestWriterThread *t(
        new TestWriterThread(
            &rw_mutex, &unblocked_count, &unblocked_count_mutex,
            &unblocked_condition,
            &finish_writing_mutex,
            &simultaneous_running_writers,
            &max_simultaneous_writers));
    pool->PushTask(NewCallback(
        t, &TestWriterThread::operator()));
  }
  {
    MutexLock lk(&unblocked_count_mutex);
    while (unblocked_count < reader_count) {
      unblocked_condition.Wait(&unblocked_count_mutex);
    }
  }
  sleep(1);
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      unblocked_count, reader_count);

  finish_reading_lock.reset();

  {
    MutexLock lk(&unblocked_count_mutex);
    while (unblocked_count < (reader_count + 1)) {
      unblocked_condition.Wait(&unblocked_count_mutex);
    }
  }
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      unblocked_count, reader_count+1);

  finish_writing_lock.reset();
  pool->Stop();
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      unblocked_count, reader_count+writer_count);
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      max_simultaneous_readers, reader_count);
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      max_simultaneous_writers, 1u);
}

TEST_F(SharedMutexTest, TestOnlyOneUpgradeLockPermitted) {
  unsigned const number_of_threads = 10;
  shared_ptr<ThreadPool> pool(
      ThreadPool::Create("TestOnlyOneUpgradeLockPermitted", number_of_threads));
  pool->Start();

  SharedMutex rw_mutex;
  unsigned unblocked_count = 0;
  unsigned simultaneous_running_count = 0;
  unsigned max_simultaneous_running = 0;
  Mutex unblocked_count_mutex;
  CondVar unblocked_condition;
  Mutex finish_mutex;
  {
    MutexLock finish_lock(&finish_mutex);

    for (unsigned i = 0; i < number_of_threads; ++i) {
      TestUpgradeThread *t(
          new TestUpgradeThread(
              &rw_mutex, &unblocked_count, &unblocked_count_mutex,
              &unblocked_condition,
              &finish_mutex,
              &simultaneous_running_count,
              &max_simultaneous_running));
      pool->PushTask(NewCallback(
          t, &TestUpgradeThread::operator()));
    }
    sleep(1);

    CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex, unblocked_count, 1U);
  }
  pool->Stop();
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      unblocked_count, number_of_threads);
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      max_simultaneous_running, 1u);
}

TEST_F(SharedMutexTest, TestCanLockUpgradeIfCurrentlyLockedShared) {
  unsigned const number_of_threads = 11;
  unsigned const reader_count = 10;
  shared_ptr<ThreadPool> pool(
      ThreadPool::Create("TestOnlyOneUpgradeLockPermitted", number_of_threads));
  pool->Start();

  SharedMutex rw_mutex;
  unsigned unblocked_count = 0;
  unsigned simultaneous_running_count = 0;
  unsigned max_simultaneous_running = 0;
  Mutex unblocked_count_mutex;
  CondVar unblocked_condition;
  Mutex finish_mutex;
  {
    MutexLock finish_lock(&finish_mutex);

    for (unsigned i = 0; i < reader_count; ++i) {
      TestReaderThread *t(
          new TestReaderThread(
              &rw_mutex, &unblocked_count, &unblocked_count_mutex,
              &unblocked_condition,
              &finish_mutex,
              &simultaneous_running_count,
              &max_simultaneous_running));
      pool->PushTask(NewCallback(
          t, &TestReaderThread::operator()));
    }
    sleep(1);
    TestUpgradeThread *t(
        new TestUpgradeThread(
            &rw_mutex, &unblocked_count, &unblocked_count_mutex,
            &unblocked_condition,
            &finish_mutex,
            &simultaneous_running_count,
            &max_simultaneous_running));
    pool->PushTask(NewCallback(
        t, &TestUpgradeThread::operator()));
    {
      MutexLock lk(&unblocked_count_mutex);
      while (unblocked_count < (reader_count + 1)) {
        unblocked_condition.Wait(&unblocked_count_mutex);
      }
    }
    CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
        unblocked_count, reader_count+1);
  }
  pool->Stop();
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      unblocked_count, reader_count + 1);
  CHECK_LOCKED_VALUE_EQUAL(unblocked_count_mutex,
      max_simultaneous_running, reader_count + 1);
}

TEST_F(SharedMutexTest,
    TestIfOtherThreadHasWriteLockTryLockSharedReturnsFalse) {
  SharedMutex rw_mutex;
  Mutex finish_mutex;
  Mutex unblocked_mutex;
  unsigned unblocked_count = 0;
  SimpleWritingThread *writing(new SimpleWritingThread(
      &rw_mutex, &finish_mutex, &unblocked_mutex, &unblocked_count));

  Thread writer("writer",
                NewCallback(writing, &SimpleWritingThread::operator()));
  {
    ASSERT_TRUE(rw_mutex.TrylockShared());
    rw_mutex.UnlockShared();
    MutexLock finish_lock(&finish_mutex);
    writer.Start();
    sleep(1);
    CHECK_LOCKED_VALUE_EQUAL(unblocked_mutex, unblocked_count, 1u);

    ASSERT_FALSE(rw_mutex.TrylockShared());
  }
  writer.Join();
}

TEST_F(SharedMutexTest,
    TestIfOtherThreadHasWriteLockTryLockUpgradeReturnsFalse) {
  SharedMutex rw_mutex;
  Mutex finish_mutex;
  Mutex unblocked_mutex;
  unsigned unblocked_count = 0;
  SimpleWritingThread *writing(new SimpleWritingThread(
      &rw_mutex, &finish_mutex, &unblocked_mutex, &unblocked_count));

  Thread writer("writer",
                NewCallback(writing, &SimpleWritingThread::operator()));
  {
    MutexLock finish_lock(&finish_mutex);
    ASSERT_TRUE(rw_mutex.TrylockUpgrade());
    rw_mutex.UnlockUpgrade();
    writer.Start();
    sleep(1);
    CHECK_LOCKED_VALUE_EQUAL(unblocked_mutex, unblocked_count, 1u);

    ASSERT_FALSE(rw_mutex.TrylockUpgrade());
  }
  writer.Join();
}

TEST_F(SharedMutexTest,
    TestIfOtherThreadHasSharedLockTryLockUpgradeReturnsTrue) {
  SharedMutex rw_mutex;
  Mutex finish_mutex;
  Mutex unblocked_mutex;
  unsigned unblocked_count = 0;
  SimpleReadingThread *writing(new SimpleReadingThread(
      &rw_mutex, &finish_mutex, &unblocked_mutex, &unblocked_count));

  Thread reader("reader",
                NewCallback(writing, &SimpleReadingThread::operator()));
  {
    ASSERT_TRUE(rw_mutex.TrylockShared());
    rw_mutex.UnlockShared();
    MutexLock finish_lock(&finish_mutex);
    reader.Start();
    sleep(1);
    CHECK_LOCKED_VALUE_EQUAL(unblocked_mutex, unblocked_count, 1u);

    ASSERT_TRUE(rw_mutex.TrylockShared());
    ASSERT_TRUE(rw_mutex.TrylockUpgrade());
    rw_mutex.UnlockShared();
    rw_mutex.UnlockUpgrade();
  }
  reader.Join();
}

TEST_F(SharedMutexTest,
       TestIfOtherThreadHasUpgradeLockTryLockUpgradeReturnsFalse) {
  SharedMutex rw_mutex;
  Mutex finish_mutex;
  Mutex unblocked_mutex;
  unsigned unblocked_count = 0;
  SimpleUpgradeThread *writing(new SimpleUpgradeThread(
      &rw_mutex, &finish_mutex, &unblocked_mutex, &unblocked_count));

  Thread upgrader("upgrader",
                NewCallback(writing, &SimpleUpgradeThread::operator()));
  {
    ASSERT_TRUE(rw_mutex.TrylockUpgrade());
    rw_mutex.UnlockUpgrade();
    MutexLock finish_lock(&finish_mutex);
    upgrader.Start();
    sleep(1);
    CHECK_LOCKED_VALUE_EQUAL(unblocked_mutex, unblocked_count, 1u);

    ASSERT_FALSE(rw_mutex.TrylockUpgrade());
  }
  upgrader.Join();
}

TEST_F(SharedMutexTest, TestTimedLockSharedTimesOutIfWriteLockHeld) {
  static const int timeout_ms[] = {50, 500};
  SharedMutex rw_mutex;
  for (int i = 0; i < arraysize(timeout_ms); ++i) {
    {
      double now = WallTime_Now() * 1000;
      ASSERT_TRUE(rw_mutex.TimedLockShared(timeout_ms[i]));
      double cur = WallTime_Now() * 1000;
      EXPECT_LT(cur, now + timeout_ms[i]);
      rw_mutex.UnlockShared();
    }
    {
      double now = WallTime_Now() * 1000;
      ASSERT_TRUE(rw_mutex.TimedLock(timeout_ms[i]));
      double cur = WallTime_Now() * 1000;
      EXPECT_LT(cur, now + timeout_ms[i]);
      rw_mutex.Unlock();
    }
    {
      double now = WallTime_Now() * 1000;
      ASSERT_TRUE(rw_mutex.TimedLockUpgrade(timeout_ms[i]));
      double cur = WallTime_Now() * 1000;
      EXPECT_LT(cur, now + timeout_ms[i]);
      rw_mutex.UnlockUpgrade();
    }
  }
  Mutex finish_mutex;
  Mutex unblocked_mutex;
  unsigned unblocked_count = 0;
  SimpleWritingThread *writing(new SimpleWritingThread(
      &rw_mutex, &finish_mutex, &unblocked_mutex, &unblocked_count));

  Thread writer("writer",
                NewCallback(writing, &SimpleWritingThread::operator()));
  {
    MutexLock finish_lock(&finish_mutex);
    writer.Start();
    sleep(1);
    CHECK_LOCKED_VALUE_EQUAL(unblocked_mutex, unblocked_count, 1u);

    for (int i = 0; i < arraysize(timeout_ms); ++i) {
      {
        double now = WallTime_Now() * 1000;
        ASSERT_FALSE(rw_mutex.TimedLockShared(timeout_ms[i]));
        double cur = WallTime_Now() * 1000;
        EXPECT_NEAR(now + timeout_ms[i], cur, 10);
      }
      {
        double now = WallTime_Now() * 1000;
        ASSERT_FALSE(rw_mutex.TimedLock(timeout_ms[i]));
        double cur = WallTime_Now() * 1000;
        EXPECT_NEAR(now + timeout_ms[i], cur, 10);
      }
      {
        double now = WallTime_Now() * 1000;
        ASSERT_FALSE(rw_mutex.TimedLockUpgrade(timeout_ms[i]));
        double cur = WallTime_Now() * 1000;
        EXPECT_NEAR(now + timeout_ms[i], cur, 10);
      }
    }
  }
  writer.Join();
}
