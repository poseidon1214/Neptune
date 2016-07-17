// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/05/11
// Description: test binary version

#include "common/base/binary_version.h"
#include "thirdparty/gtest/gtest.h"

TEST(BinaryVersion, Test) {
  EXPECT_STRNE("Unknown", binary_version::kBuilderName);
  EXPECT_STRNE("Unknown", binary_version::kBuildTime);
  EXPECT_STRNE("Unknown", binary_version::kHostName);
  EXPECT_STRNE("Unknown", binary_version::kCompiler);
  // EXPECT_NE(0, binary_version::kSvnInfoCount);
}
