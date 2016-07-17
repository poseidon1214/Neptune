// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/base/gflags_in_spp.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/gtest/gtest.h"

DEFINE_int32(flag1, 0, "flag1");
DEFINE_int32(flag2, 0, "flag2");

namespace gdt {

TEST(GflagsInSpp, Init) {
  InitGflagsInSpp("test1.flags");
  EXPECT_EQ(1, FLAGS_flag1);
}

TEST(GflagsInSpp, InitMultiple) {
  InitGflagsInSpp("test1.flags test2.flags");
  EXPECT_EQ(1, FLAGS_flag1);
  EXPECT_EQ(2, FLAGS_flag2);
}

TEST(GflagsInSpp, InitNonExisted) {
  ASSERT_EXIT(InitGflagsInSpp("test3.flags"), ::testing::ExitedWithCode(1),
              "No such file or directory");
}

}  // namespace gdt
