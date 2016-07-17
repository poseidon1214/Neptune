// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Authors: CHEN Feng <phongchen@tencent.com>
//          Wang Yu <wangyu@tencent.com>
// Created: 2011-08-31
// Description: thread local storage test

#include "common/system/concurrency/thread_local.h"

#include <limits.h>
#include <pthread.h>

#include <complex>
#include <stdexcept>
#include <string>
#include <vector>

#include "app/qzap/common/base/thread.h"

#include "thirdparty/gtest/gtest.h"

namespace gdt {
void Inc(ThreadLocalValue<int>* n, int* main_ptr) {
  ++n->Value();
  EXPECT_EQ(1, n->Value());
  EXPECT_NE(&n->Value(), main_ptr);
}

TEST(ThreadLocalValue, Simple) {
  static ThreadLocalValue<int> n(0);
  EXPECT_EQ(0, n.Value());

  Thread thread(NewCallback(Inc, &n, &n.Value()));
  thread.Start();
  thread.Join();
  EXPECT_EQ(0, n.Value());
}

TEST(ThreadLocalValue, Single) {
  static ThreadLocalValue<int> n(0);
  n = 0;
}

TEST(ThreadLocalValue, Complex) {
  static ThreadLocalValue<std::complex<double> > n(0, 0);
  n = std::complex<double>(1, 2);
  static ThreadLocalValue<std::string> s("ssssssssssss");
  std::cout << s.Value() << "\n";
}

TEST(ThreadLocalPtr, Simple) {
  static ThreadLocalPtr<int> p;
  EXPECT_EQ((int*)NULL, p.Get());
  p.Reset(new int(1));
  EXPECT_EQ(1, *p);
  p.Reset();
  EXPECT_EQ((int*)NULL, p.Get());
}

TEST(ThreadLocalPtr, WithProtoyype) {
  static ThreadLocalPtr<int> p(new int());
  EXPECT_NE((int*)NULL, p.Get());
  p.Reset();
  EXPECT_EQ((int*)NULL, p.Get());
}

TEST(ThreadLocalPtr, WithAutoCreate) {
  static ThreadLocalPtr<int> p(ThreadLocal::AUTO_CREATE);
  EXPECT_NE((int*)NULL, p.Get());
  p.Reset();
  EXPECT_EQ((int*)NULL, p.Get());
}

class UncopyableFoo : private Uncopyable {
};

TEST(ThreadLocalPtr, Uncopyable) {
  static ThreadLocalPtr<UncopyableFoo> p1;
  static ThreadLocalPtr<UncopyableFoo> p2(ThreadLocal::AUTO_CREATE);

  // uncompile test, uncomment to test
  // static ThreadLocalPtr<UncopyableFoo> p3(new UncopyableFoo);
}
}
