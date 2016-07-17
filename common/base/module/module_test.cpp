// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/19/11
// Description:

#include "common/base/module.h"
#include "common/base/module/module_a.h"
#include "common/base/module/module_b.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

// namespace common {

TEST(Module, Test) {
  EXPECT_EQ(1, GetA());
  EXPECT_EQ(2, GetB());
}

int main(int argc, char** argv) {
  InitAllModulesAndTest(&argc, &argv);
  InitAllModules(&argc, &argv);  // Test Init twice
  return RUN_ALL_TESTS();
}

// } // namespace common
