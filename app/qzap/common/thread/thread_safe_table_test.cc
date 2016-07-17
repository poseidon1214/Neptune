// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
#include <sstream>
#include "app/qzap/common/base/sysinfo.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/gtest/gtest.h"
#include "app/qzap/common/thread/count_blocker.h"
#include "app/qzap/common/utility/time_utility.h"
#include "app/qzap/common/thread/thread_safe_table.h"
#include "app/qzap/common/thread/threadpool.h"

typedef ThreadSafeTable<int, int> TestTable;
class ThreadSafeTableTest : public testing::Test {
 public:
  void Insert(TestTable *table, int64_t i, CountBlocker *count) {
    table->Insert(i, i + 1);
    count->Dec(1);
  }
  void FindAndRemove(TestTable *table, int64_t i, CountBlocker *count) {
    int j;
    while (!table->FindAndRemove(i, &j)) {
      SchedYield();
    }
    EXPECT_EQ(i + 1, j);
    count->Dec(1);
  }
  void Remove(TestTable *table, int64_t i, CountBlocker *count) {
    int j;
    while (!table->Find(i, NULL)) {
      SchedYield();
    }
    EXPECT_TRUE(table->Find(i, &j));
    EXPECT_EQ(i + 1, j);
    table->Remove(i);
    count->Dec(1);
  }
 protected:
  static const int kPoolSize = 100;
  static const int64_t kTestSize = 10000;
};

TEST_F(ThreadSafeTableTest, Test1) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("Test", kPoolSize));
  TestTable table;
  p->Start();
  CountBlocker count(1);
  int64_t remove_count = 0;
  for (int64_t i = 0; i < kTestSize; ++i) {
    count.Inc(1);
    p->PushTask(NewCallback(
        this, &ThreadSafeTableTest::Insert, &table, i, &count));
    if (i % 3) {
      remove_count++;
      count.Inc(1);
      p->PushTask(NewCallback(
          this, &ThreadSafeTableTest::Remove, &table, i, &count));
    }
  }
  count.Dec(1);
  count.Wait();
  ASSERT_EQ(table.Size(), kTestSize - remove_count);
  for (TestTable::const_iterator i = table.begin();
       i != table.end(); ++i) {
    EXPECT_GT((i->second) % 3, 0);
  }
}

TEST_F(ThreadSafeTableTest, Test2) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("Test", kPoolSize));
  TestTable table;
  p->Start();
  CountBlocker count(1);
  int64_t remove_count = 0;
  for (int64_t i = 0; i < kTestSize; ++i) {
    count.Inc(1);
    p->PushTask(NewCallback(
        this, &ThreadSafeTableTest::Insert, &table, i, &count));
    if (i % 3) {
      remove_count++;
      count.Inc(1);
      p->PushTask(NewCallback(
          this, &ThreadSafeTableTest::FindAndRemove, &table, i, &count));
    }
  }
  count.Dec(1);
  count.Wait();
  ASSERT_EQ(table.Size(), kTestSize - remove_count);
  for (TestTable::const_iterator i = table.begin();
       i != table.end(); ++i) {
    EXPECT_GT((i->second) % 3, 0);
  }
}

typedef LockableTable<int, int> TestTable2;
class LockableTableTest : public testing::Test {
 public:
  void Insert(TestTable2 *table, int64_t i, CountBlocker *count) {
    TestTable2::ScopedLock locker(table);
    table->Insert(i, i + 1);
    count->Dec(1);
  }
  void FindAndRemove(TestTable2 *table, int64_t i, CountBlocker *count) {
    int j;
    table->Lock();
    while (!table->FindAndRemove(i, &j)) {
      table->Unlock();
      SchedYield();
      table->Lock();
    }
    table->Unlock();
    EXPECT_EQ(i + 1, j);
    count->Dec(1);
  }
  void Remove(TestTable2 *table, int64_t i, CountBlocker *count) {
    int j;
    table->LockShared();
    while (!table->Find(i, NULL)) {
      table->UnlockShared();
      SleepMilliseconds(100);
      table->LockShared();
    }
    EXPECT_TRUE(table->Find(i, &j));
    table->UnlockShared();
    EXPECT_EQ(i + 1, j);
    TestTable2::ScopedLock locker(table);
    table->Remove(i);
    count->Dec(1);
  }
 protected:
  static const int kPoolSize = 100;
  static const int64_t kTestSize = 10000;
};

TEST_F(LockableTableTest, Test1) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("Test", kPoolSize));
  TestTable2 table;
  p->Start();
  CountBlocker count(1);
  int64_t remove_count = 0;
  for (int64_t i = 0; i < kTestSize; ++i) {
    count.Inc(1);
    p->PushTask(NewCallback(
        this, &LockableTableTest::Insert, &table, i, &count));
    if (i % 3) {
      remove_count++;
      count.Inc(1);
      p->PushTask(NewCallback(
          this, &LockableTableTest::Remove, &table, i, &count));
    }
  }
  count.Dec(1);
  count.Wait();
  ASSERT_EQ(table.Size(), kTestSize - remove_count);
  for (TestTable2::const_iterator i = table.begin();
       i != table.end(); ++i) {
    EXPECT_GT((i->second) % 3, 0);
  }
}

TEST_F(LockableTableTest, Test2) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("Test2", kPoolSize));
  TestTable2 table;
  p->Start();
  CountBlocker count(1);
  int64_t remove_count = 0;
  for (int64_t i = 0; i < kTestSize; ++i) {
    count.Inc(1);
    p->PushTask(NewCallback(
        this, &LockableTableTest::Insert, &table, i, &count));
    if (i % 3) {
      remove_count++;
      count.Inc(1);
      p->PushTask(NewCallback(
          this, &LockableTableTest::FindAndRemove, &table, i, &count));
    }
  }
  count.Dec(1);
  count.Wait();
  ASSERT_EQ(table.Size(), kTestSize - remove_count);
  for (TestTable2::const_iterator i = table.begin();
       i != table.end(); ++i) {
    EXPECT_GT((i->second) % 3, 0);
  }
}

int main(int argc, char **argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
