// Copyright (c) 2014 Tencent Inc.
// Author: Li Meng (elvisli@tencent.com)

#include "common/config/base_config_manager.h"
#include "common/config/test_config.pb.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

typedef BaseConfigManager<TestConfig> TestConfigMgr;

TEST(BaseConfigManager, TestConfigMgr) {
  ASSERT_TRUE(TestConfigMgr::Instance().Load("testdata/test_config.txt"));
  const TestConfig& test_config = TestConfigMgr::Instance().Get();
  EXPECT_EQ(123, test_config.key());
  EXPECT_EQ("abc", test_config.value());
}

}  // namespace gdt
