// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 12/14/11
// Description: test for STATIC_ASSERT

#include "common/base/static_assert.h"
#include "thirdparty/gtest/gtest.h"

TEST(StaticAssert, Test) {
  STATIC_ASSERT(1 == 1);
  STATIC_ASSERT(1 == 1, "1 should be equal to 1");
}

TEST(StaticAssert, NoCompileTest) {
#if 0  // uncomment to test
  STATIC_ASSERT(false);
  STATIC_ASSERT(1 == 2);
  STATIC_ASSERT(1 == 2, "1 == 2");
#endif
}
