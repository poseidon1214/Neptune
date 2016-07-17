// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 12/14/11
// Description: test for preprocess.h

#include "common/base/preprocess.h"
#include "common/base/preprocess_test_helper.h"
#include "thirdparty/gtest/gtest.h"

TEST(Preprocess, Stringize) {
  EXPECT_STREQ("ABC", PP_STRINGIZE(ABC));
}

TEST(Preprocess, Join) {
  EXPECT_EQ(12, PP_JOIN(1, 2));
}

TEST(Preprocess, DisallowInHeader) {
  PP_DISALLOW_IN_HEADER_FILE();
}


