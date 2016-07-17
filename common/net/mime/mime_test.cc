// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: YU Shizhan <stanyu@tencent.com>
//

#include "common/net/mime/mime.h"
#include "thirdparty/gtest/gtest.h"

using namespace gdt;

TEST(MimeType, FromFileExtension) {
  MimeType mt = MimeType::FromFileExtension("html");
  EXPECT_EQ("text/html", mt.ToString());
}


TEST(MimeType, Set) {
  MimeType mt;
  EXPECT_TRUE(mt.Set("text/xml"));
  EXPECT_EQ("text/xml", mt.ToString());
  EXPECT_FALSE(mt.Set("invalid"));
}

TEST(MimeType, Match) {
  MimeType mt;
  mt.Set("text/xml");
  EXPECT_TRUE(mt.Match("text/xml"));
  EXPECT_TRUE(mt.Match("*/xml"));
  EXPECT_TRUE(mt.Match("text/*"));
  EXPECT_FALSE(mt.Match("textxml"));
}
