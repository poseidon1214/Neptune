// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 08/04/11
// Description:

#include "common/base/string/compare.h"
#include "thirdparty/gtest/gtest.h"

// namespace common {

TEST(StringCompare, ByteString) {
  std::string str1 = "abcdefgXXXXX";
  std::string str2 = "abcdefg";
  std::string str3 = "abcdefgYYYYY";

  bool inclusive;
  ASSERT_GT(CompareByteString(
              str1.data(), str1.length(),
              str2.data(), str2.length(),
              &inclusive), 0);

  ASSERT_TRUE(inclusive);

  ASSERT_GT(CompareByteString(str1, str2), 0);
  ASSERT_LT(CompareByteString(str2, str1), 0);
  ASSERT_EQ(CompareByteString(str2, str2), 0);
  ASSERT_LT(CompareByteString(str1, str3), 0);
  ASSERT_GT(CompareByteString(str3, str2), 0);

  ASSERT_EQ(7U, GetCommonPrefixLength(str1, str3));
}

const char g_s1[] =
  "http://finance.qq.com/a/20101228/http://finance.qq.com/a/20101228/006335.htm";
const char g_s2[] =
  "http://finance.qq.com/a/20101228/http://finance.qq.com/a/20101228/006336.htm";

TEST(StringCompare, GetCommonPrefixLength) {
  std::string str1 = "abcdefgXXXXX";
  std::string str2 = "abcdefg";
  std::string str3 = "abcdefgYYYYY";
  ASSERT_EQ(7U, GetCommonPrefixLength(str1, str2));
  ASSERT_EQ(7U, GetCommonPrefixLength(str1, str3));

  const char* volatile s1 = g_s1;
  const char* volatile s2 = g_s2;
  int total = 0;
  for (int i = 0; i < 1000000; ++i)
    total += GetCommonPrefixLength(s1, sizeof(g_s1) - 1, s2, sizeof(g_s2) - 1);
  volatile int n = total;
  (void) n;
}

TEST(String, IgnoreCase) {
  EXPECT_EQ(0, CompareStringIgnoreCase("ABC", "abc"));
  EXPECT_GT(CompareStringIgnoreCase("abcd", "abc"), 0);
  EXPECT_LT(CompareStringIgnoreCase("abc", "abcd"), 0);
  EXPECT_TRUE(StringEqualsIgnoreCase("abc", "ABC"));
  EXPECT_FALSE(StringEqualsIgnoreCase("abc", "ABD"));
}

TEST(StringCompare, Performance) {
  const char* volatile s1 = g_s1;
  const char* volatile s2 = g_s2;
  int total = 0;
  for (int i = 0; i < 1000000; ++i)
    total += CompareByteString(s1, sizeof(g_s1) - 1, s2, sizeof(g_s2) - 1);
  volatile int n = total;
  (void) n;
}

TEST(StringCompare, memcmp_Performance) {
  const char* volatile s1 = g_s1;
  const char* volatile s2 = g_s2;
  int total = 0;
  for (int i = 0; i < 1000000; ++i) {
    total += memcmp(s1, s2, sizeof(g_s1) - 1);
  }
  volatile int n = total;
  (void) n;
}

TEST(StringCompare, CompareMemory_Performance) {
  const char* volatile s1 = g_s1;
  const char* volatile s2 = g_s2;
  int total = 0;
  for (int i = 0; i < 1000000; ++i) {
    total += CompareMemory(s1, s2, sizeof(g_s1) - 1);
  }
  volatile int n = total;
  (void) n;
}

TEST(StringCompare, MemoryEqual_Performance) {
  const char* volatile s1 = g_s1;
  const char* volatile s2 = g_s2;
  int total = 0;
  for (int i = 0; i < 1000000; ++i) {
    total += MemoryEqual(s1, s2, sizeof(g_s1) - 1);
  }
  volatile int n = total;
  (void) n;
}

// } // namespace common
