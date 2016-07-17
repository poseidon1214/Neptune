// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>

#include "thirdparty/gtest/gtest.h"
#include "app/qzap/common/utility/mobile_application_utility.h"

TEST(MobileApplication, GenerateMobileApplicationKeyId) {
  EXPECT_EQ(GenerateMobileApplicationKeyId("hello", "1"), 301171429u);
  EXPECT_EQ(GenerateMobileApplicationKeyId("hello", "2"), 3781299451u);
  EXPECT_EQ(GenerateMobileApplicationKeyId("hello", "3"), 3370065142u);
}

TEST(MobileApplication, GenerateMobileApplicationPackageNameId) {
  EXPECT_EQ(GenerateMobileApplicationPackageNameId("hello_1"), 301171429u);
  EXPECT_EQ(GenerateMobileApplicationPackageNameId("hello_2"), 3781299451u);
  EXPECT_EQ(GenerateMobileApplicationPackageNameId("hello_3"), 3370065142u);
}
