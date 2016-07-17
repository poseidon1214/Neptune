// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22

#include "common/base/shared_ptr.h"

#include <tr1/unordered_set>
#include <set>
#include <vector>

#include "common/system/concurrency/atomic/atomic.h"
#include "common/system/concurrency/thread.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

namespace gdt {

class SharedPtrTest : public testing::Test {
};

static void Inc(shared_ptr<int> s, std::vector<shared_ptr<int> > *v) {
  volatile int *i = s.get();
  int j = AtomicIncrement(i);
  (*v)[j] = s;
}

TEST_F(SharedPtrTest, Test0) {
  shared_ptr<int> s(new int);
  ASSERT_EQ(s.use_count(), 1);
  shared_ptr<int> s2 = s;
  ASSERT_EQ(s2.use_count(), 2);
}

TEST_F(SharedPtrTest, Test1) {
  shared_ptr<int> s(new int);
  ASSERT_EQ(s.use_count(), 1);
  weak_ptr<int> w(s);
  ASSERT_EQ(s.use_count(), 1);
  weak_ptr<int> w1(w);
  ASSERT_EQ(s.use_count(), 1);
  weak_ptr<int> w2(w1);
  ASSERT_EQ(s.use_count(), 1);
  shared_ptr<int> s2 = w.lock();
  ASSERT_EQ(s2.use_count(), 2);
  weak_ptr<int> w3(s2);
  shared_ptr<int> s3 = w3.lock();
  ASSERT_EQ(s2.use_count(), 3);
}

TEST_F(SharedPtrTest, Test2) {
  static const int kThreadNumber = 200;
  shared_ptr<int> s(new int(0));
  std::vector<shared_ptr<Thread> > threads;
  std::vector<shared_ptr<int> > ss;
  ss.resize(kThreadNumber + 1);
  for (int i = 0; i < kThreadNumber; ++i) {
    shared_ptr<Thread> t(new Thread(NewCallback(Inc, s, &ss)));
    threads.push_back(t);
    threads.back()->Start();
  }
  for (int i = 0; i < kThreadNumber; ++i) {
    threads[i]->Join();
  }
  ASSERT_EQ(s.use_count(), kThreadNumber + 1);
  ASSERT_EQ(*s.get(), kThreadNumber);
}

TEST_F(SharedPtrTest, HashTest) {
  shared_ptr<int> s1(new int(0));
  shared_ptr<int> s2(new int(0));
  shared_ptr<int> s3 = s1;
  std::tr1::unordered_set<shared_ptr<int> > set;
  set.insert(s1);
  set.insert(s2);
  set.insert(s3);
  ASSERT_EQ(set.size(), static_cast<size_t>(2));
}

TEST_F(SharedPtrTest, BoolTest) {
  shared_ptr<int> s1(new int(0));
  shared_ptr<int> s2;
  EXPECT_TRUE(s1);
  EXPECT_FALSE(s2);
  EXPECT_FALSE(!s1);
  EXPECT_TRUE(!s2);
}

TEST_F(SharedPtrTest, Equal) {
  shared_ptr<int> s1(new int(0));
  shared_ptr<int> s2(s1);
  shared_ptr<int> s3;
  EXPECT_EQ(s1, s2);
  EXPECT_EQ(s2, s1);
  EXPECT_NE(s1, s3);
  EXPECT_NE(s3, s1);
}

class TestBase {
 public:
  TestBase() { }
  virtual ~TestBase() { }
};

class TestDerive : public TestBase {
};

class TestTmp {
};

TEST_F(SharedPtrTest, DynamicPointerCast) {
  shared_ptr<TestBase> derive1(new TestDerive());
  ASSERT_EQ(1, derive1.use_count());
  shared_ptr<TestDerive> derive2 = dynamic_pointer_cast<TestDerive>(derive1);
  ASSERT_NE(reinterpret_cast<TestDerive*>(NULL), derive2.get());
  ASSERT_EQ(2, derive1.use_count());
  ASSERT_EQ(2, derive2.use_count());
  ASSERT_EQ(derive2.get(), derive1.get());
  shared_ptr<TestTmp> tmp = dynamic_pointer_cast<TestTmp>(derive2);
  ASSERT_EQ(reinterpret_cast<TestTmp*>(NULL), tmp.get());
}

}  // namespace gdt
