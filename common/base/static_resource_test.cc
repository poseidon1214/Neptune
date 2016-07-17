// Copyright (c) 2015, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/base/static_resource.h"
#include "common/base/static_resource_test_data.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

TEST(StaticResource, Package) {
  const StaticResourcePackage& package =
      STATIC_RESOURCE_PACKAGE(common_base_static_resource_test_data);
  StringPiece data;
  ASSERT_TRUE(package.Find("static_resource.testdata", &data));
  EXPECT_EQ("hello, blade!\n", data);
  ASSERT_FALSE(package.Find("wtf", &data));
}

}  // namespace gdt
