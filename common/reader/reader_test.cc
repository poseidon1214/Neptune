// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
// Author: Wang Qian <cernwang@tencent.com>

#include <string>
#include <map>
#include <utility>

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/utility/file_utility.h"
#include "common/reader/reader.h"
#include "common/config/base_config_manager.h"

using namespace gdt;
typedef BaseConfigManager<IOConfig> ConfigChecker;

std::string AddZero(const std::string& str) {
  return str + "0";
}

TEST(Reader, LineReader) {
  REGISTER_FUNC(AddZero);
  std::vector<Click> clicks;
  ASSERT_TRUE(ConfigChecker::Instance().Load("line_reader.conf"));
  EXPECT_EQ(Reader::ReadFromIO(ConfigChecker::Instance().Get(), &clicks), true);
  EXPECT_EQ(clicks.size(), 3);
  EXPECT_EQ(clicks[0].user().qq(), 3);
  EXPECT_EQ(clicks[1].user().qq(), 2);
  EXPECT_EQ(clicks[2].user().qq(), 2);
  EXPECT_EQ(clicks[0].process_time(), 6);
  EXPECT_EQ(clicks[1].process_time(), 1);
  EXPECT_EQ(clicks[2].process_time(), 1);
  EXPECT_EQ(clicks[0].advertiser_id(), 10);
  EXPECT_EQ(clicks[1].advertiser_id(), 40);
  EXPECT_EQ(clicks[2].advertiser_id(), 30);
}

TEST(Reader, XmlReader) {
  std::vector<Click> clicks;
  ASSERT_TRUE(ConfigChecker::Instance().Load("xml_reader.conf"));
  EXPECT_EQ(Reader::ReadFromIO(ConfigChecker::Instance().Get(), &clicks), true);
  EXPECT_EQ(clicks.size(), 4);
  EXPECT_EQ(clicks[0].user().qq(), 15);
  EXPECT_EQ(clicks[1].user().qq(), 2999);
  EXPECT_EQ(clicks[2].user().qq(), 1999);
  EXPECT_EQ(clicks[0].process_time(), 56591);
  EXPECT_EQ(clicks[1].process_time(), 1232);
  EXPECT_EQ(clicks[2].process_time(), 12345);
  EXPECT_EQ(clicks[0].advertiser_id(), 12);
  EXPECT_EQ(clicks[1].advertiser_id(), 1999);
  EXPECT_EQ(clicks[2].advertiser_id(), 515);
}

