// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/encoding/base64.h"
#include "thirdparty/gtest/gtest.h"

const std::string kText = ".<>@???????";
const std::string kText2 = ".<>@????????";
const std::string kBase64Text = "Ljw+QD8/Pz8/Pz8=";
const std::string kRfcWebSafeBase64Text = "Ljw-QD8_Pz8_Pz8=";
const std::string kPaddingSkippedRfcWebSafeBase64Text = "Ljw-QD8_Pz8_Pz8";
const std::string kPaddingSkippedRfcWebSafeBase64Text2 = "Ljw-QD8_Pz8_Pz8_";

// namespace common {

TEST(Base64Test, Encode) {
  std::string result;
  EXPECT_TRUE(Base64::Encode(kText, &result));
  EXPECT_EQ(kBase64Text, result);
}

TEST(Base64Test, Decode) {
  std::string result;
  EXPECT_TRUE(Base64::Decode(kBase64Text, &result));
  EXPECT_EQ(kText, result);
}

TEST(Base64Test, RfcWebSafeEncode) {
  std::string result;
  EXPECT_TRUE(Base64::RfcWebSafeEncode(kText, &result));
  EXPECT_EQ(kRfcWebSafeBase64Text, result);
}

TEST(Base64Test, RfcWebSafeDecode) {
  std::string result;
  EXPECT_FALSE(Base64::Decode(kRfcWebSafeBase64Text, &result));
  EXPECT_TRUE(Base64::RfcWebSafeDecode(kRfcWebSafeBase64Text, &result));
  EXPECT_EQ(kText, result);
}

TEST(Base64Test, PaddingSkippedRfcWebSafeEncode) {
  std::string result;
  EXPECT_TRUE(Base64::PaddingSkippedRfcWebSafeEncode(kText, &result));
  EXPECT_EQ(kPaddingSkippedRfcWebSafeBase64Text, result);
  EXPECT_TRUE(Base64::PaddingSkippedRfcWebSafeEncode(kText2, &result));
  EXPECT_EQ(kPaddingSkippedRfcWebSafeBase64Text2, result);
}

TEST(Base64Test, PaddingSkippedRfcWebSafeDecode) {
  std::string result;
  EXPECT_FALSE(Base64::Decode(kPaddingSkippedRfcWebSafeBase64Text, &result));
  EXPECT_FALSE(Base64::RfcWebSafeDecode(kPaddingSkippedRfcWebSafeBase64Text,
                                        &result));
  EXPECT_TRUE(Base64::PaddingSkippedRfcWebSafeDecode(
          kPaddingSkippedRfcWebSafeBase64Text, &result));
  EXPECT_EQ(kText, result);
  EXPECT_TRUE(Base64::PaddingSkippedRfcWebSafeDecode(
          kPaddingSkippedRfcWebSafeBase64Text2, &result));
  EXPECT_EQ(kText2, result);
}
// } // namespace common
