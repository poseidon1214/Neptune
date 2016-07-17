// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-4-8
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include "app/qzap/common/utility/uuid_utility.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
class UUIDUtilityTest : public testing::Test {
};

TEST_F(UUIDUtilityTest, Test1) {
  std::string gen_s("8982fbcd-8444-4b12-8c80-9bee0bb1ec1f");
  std::string s1 = GenerateUUID();
  std::string s2 = GenerateUUID();
  ASSERT_NE(s1, s2);
  LOG(INFO) << "s1: " << s1 << " s2: " << s2;
  ASSERT_EQ(s1.size(), gen_s.size());
  ASSERT_EQ(s2.size(), gen_s.size());
}
