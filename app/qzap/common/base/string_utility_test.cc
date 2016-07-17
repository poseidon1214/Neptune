// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include "app/qzap/common/base/base.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

class StringUtilityTest : public testing::Test {
};

TEST_F(StringUtilityTest, TestStringPrintfPRI) {
  // 0x7FFFFFFFFFFFFFFF
  int64_t i_64 = static_cast<int64_t>(9223372036854775807LL);
  std::string i64_str = "9223372036854775807";

  // 0xFFFFFFFFFFFFFFFF
  uint64_t ui_64 = static_cast<uint64_t>(18446744073709551615ULL);
  std::string ui64_str = "18446744073709551615";
  if (sizeof(int) == 8) {
    ASSERT_EQ(i64_str, StringPrintf("%ld", i_64));
    ASSERT_EQ(ui64_str, StringPrintf("%lu", ui_64));
  }
}

TEST_F(StringUtilityTest, Test2) {
  std::string s1("");
  std::vector<std::string> v1;
  ASSERT_FALSE(SplitString(s1, "/", &v1));
  ASSERT_EQ(0, static_cast<int>(v1.size()));
  ASSERT_FALSE(SplitStringByString(s1, "/", &v1));
  ASSERT_EQ(0, static_cast<int>(v1.size()));

  std::string s2("http://www.google.com:443/login.cgi?name=xxx");
  std::vector<std::string> v2;
  ASSERT_FALSE(SplitString(s2, NULL, &v2));
  ASSERT_EQ(0, static_cast<int>(v2.size()));
  ASSERT_FALSE(SplitStringByString(s2, "", &v2));
  ASSERT_EQ(0, static_cast<int>(v2.size()));

  std::vector<std::string> v3;
  ASSERT_TRUE(SplitString(s2, ":/?=", &v3));
  ASSERT_EQ(8, static_cast<int>(v3.size()));
  ASSERT_EQ("http", v3[0]);
  ASSERT_EQ("", v3[1]);
  ASSERT_EQ("", v3[2]);
  ASSERT_EQ("www.google.com", v3[3]);
  ASSERT_EQ("443", v3[4]);
  ASSERT_EQ("login.cgi", v3[5]);
  ASSERT_EQ("name", v3[6]);
  ASSERT_EQ("xxx", v3[7]);
  ASSERT_TRUE(SplitStringByString(s2, "/", &v3));
  ASSERT_EQ(4, static_cast<int>(v3.size()));
  ASSERT_EQ("http:", v3[0]);
  ASSERT_EQ("", v3[1]);
  ASSERT_EQ("www.google.com:443", v3[2]);
  ASSERT_EQ("login.cgi?name=xxx", v3[3]);

  std::string s4("//");
  std::vector<std::string> v4;
  ASSERT_TRUE(SplitString(s4, "/=", &v4));
  ASSERT_EQ(3, static_cast<int>(v4.size()));
  ASSERT_EQ("", v4[0]);
  ASSERT_EQ("", v4[1]);
  ASSERT_EQ("", v4[2]);
  ASSERT_TRUE(SplitStringByString(s2, "//", &v4));
  ASSERT_EQ(2, static_cast<int>(v4.size()));
  ASSERT_EQ("http:", v4[0]);
  ASSERT_EQ("www.google.com:443/login.cgi?name=xxx", v4[1]);

  std::string s5("no need split");
  std::vector<std::string> v5;
  ASSERT_TRUE(SplitString(s5, "/=", &v5));
  ASSERT_EQ(1, static_cast<int>(v5.size()));
  ASSERT_EQ(s5, v5[0]);
  ASSERT_TRUE(SplitStringByString(s5, "//", &v5));
  ASSERT_EQ(1, static_cast<int>(v5.size()));
  ASSERT_EQ(s5, v5[0]);

  std::string s6("head/\xE5\x93\x88/tail");
  std::vector<std::string> v6;
  ASSERT_TRUE(SplitString(s6, "/=", &v6));
  ASSERT_EQ(3, static_cast<int>(v6.size()));
  ASSERT_EQ("head", v6[0]);
  ASSERT_EQ("\xE5\x93\x88", v6[1]);
  ASSERT_EQ("tail", v6[2]);
  ASSERT_TRUE(SplitStringByString(s6, "/", &v6));
  ASSERT_EQ(3, static_cast<int>(v6.size()));
  ASSERT_EQ("head", v6[0]);
  ASSERT_EQ("\xE5\x93\x88", v6[1]);
  ASSERT_EQ("tail", v6[2]);

  std::vector<std::string> v7;
  ASSERT_FALSE(SplitString(s6,
      "\xE5\x93\x88", &v7));
  ASSERT_TRUE(SplitStringByString(s6,
      "\xE5\x93\x88", &v7));
  ASSERT_EQ(2, static_cast<int>(v7.size()));
}

TEST_F(StringUtilityTest, Test3) {
  {
    int16_t v;
    ASSERT_TRUE(StringToNumeric("-1", &v));
    ASSERT_EQ(v, -1);
  }
  {
    int16_t v;
    ASSERT_FALSE(StringToNumeric("0xFFFFF", &v));
  }
  {
    int16_t v;
    ASSERT_TRUE(StringToNumeric("0xFFF", &v));
    ASSERT_EQ(v, 0xFFF);
  }
  {
    int32_t v;
    ASSERT_TRUE(StringToNumeric("-1", &v));
    ASSERT_EQ(v, -1);
  }
  {
    int32_t v;
    ASSERT_FALSE(StringToNumeric("0xFFFFFFFFF", &v));
  }
  {
    int32_t v;
    ASSERT_TRUE(StringToNumeric("0xFFFFFFF", &v));
    ASSERT_EQ(v, 0xFFFFFFF);
  }
  {
    uint16_t v;
    ASSERT_TRUE(StringToNumeric("0xFFFF", &v));
    ASSERT_EQ(v, 0xFFFF);
  }
  {
    uint32_t v;
    ASSERT_TRUE(StringToNumeric("0xFFFFFFFF", &v));
    ASSERT_EQ(v, 0xFFFFFFFF);
  }
  {
    int64_t v;
    ASSERT_TRUE(StringToNumeric("-1", &v));
    ASSERT_EQ(v, -1);
  }
  {
    int64_t v;
    ASSERT_TRUE(StringToNumeric("0xFFFFFFFFF", &v));
    ASSERT_EQ(v, 0xFFFFFFFFFLL);
  }
  {
    int64_t v;
    ASSERT_FALSE(StringToNumeric("0xFFFFFFFFFFFFFFFF", &v));
  }
  {
    int64_t v;
    ASSERT_TRUE(StringToNumeric("0xFFFFFFFFFFFFFFF", &v));
    ASSERT_EQ(v, 0xFFFFFFFFFFFFFFFLL);
  }
  {
    uint64_t v;
    ASSERT_TRUE(StringToNumeric("-1", &v));
    ASSERT_EQ(v, 0xFFFFFFFFFFFFFFFFLL);
  }
  {
    float v;
    ASSERT_TRUE(StringToNumeric("0.1", &v));
    ASSERT_NEAR(v, 0.1, 1E-5);
  }
  {
    double v;
    ASSERT_TRUE(StringToNumeric("0.1", &v));
    ASSERT_NEAR(v, 0.1, 1E-5);
  }
  {
    int32_t v;
    ASSERT_FALSE(StringToNumeric("0xFFFFFFF-1", &v));
  }
  {
    double v;
    ASSERT_FALSE(StringToNumeric("0.11.22e-4", &v));
  }
}

TEST_F(StringUtilityTest, Test4) {
  {
    int64_t v = 0xFFFFFFFFFFFFFFFF;
    ASSERT_EQ(ConvertToString(v), "-1");
  }
  {
    uint64_t v = 0xFFFFFFFFFFFFFFFF;
    ASSERT_EQ(ConvertToString(v), "18446744073709551615");
  }
  {
    int64_t v = -1;
    ASSERT_EQ(ConvertToString(v), "-1");
  }
  {
    uint64_t v = -1;
    ASSERT_EQ(ConvertToString(v), "18446744073709551615");
  }
  {
    int64_t v = 0x1FFFFFFFF;
    ASSERT_EQ(ConvertToString(v), "8589934591");
  }
  {
    uint64_t v = 0x1FFFFFFFF;
    ASSERT_EQ(ConvertToString(v), "8589934591");
  }
  {
    uint64_t v = 1;
    ASSERT_EQ(ConvertToString(v), "1");
  }
  {
    int64_t v = 1;
    ASSERT_EQ(ConvertToString(v), "1");
  }
  {
    int64_t v = 0;
    ASSERT_EQ(ConvertToString(v), "0");
  }
  {
    uint64_t v = 0;
    ASSERT_EQ(ConvertToString(v), "0");
  }
  {
    int32_t v = -1;
    ASSERT_EQ(ConvertToString(v), "-1");
  }
  {
    uint32_t v = -1;
    ASSERT_EQ(ConvertToString(v), "4294967295");
  }
  if (sizeof(int) == 8) {
    {
      int64_t v = -0xFFFFFFFFLL;
      ASSERT_EQ(ConvertToString(v), "-4294967295");
    }
    {
      uint64_t v = -1LL;
      ASSERT_EQ(ConvertToString(v), "18446744073709551615");
    }
  }
  {
    const std::string v = "hello";
    ASSERT_EQ(ConvertToString(v), "hello");
  }
}

TEST_F(StringUtilityTest, TestTrimString) {
  {
    std::string s1("hello world");
    std::string s2("");
    std::string s3(" hello world ");
    std::string s4(" \r\t\n ");
    std::string s5(" \r \n \t hello world \r \n \t ");
    LTrimString(&s1);
    LTrimString(&s2);
    LTrimString(&s3);
    LTrimString(&s4);
    LTrimString(&s5);
    ASSERT_EQ("hello world", s1);
    ASSERT_EQ("", s2);
    ASSERT_EQ("hello world ", s3);
    ASSERT_EQ("", s4);
    ASSERT_EQ("hello world \r \n \t ", s5);
  }
  {
    std::string s1("hello world");
    std::string s2("");
    std::string s3(" hello world ");
    std::string s4(" \r\t\n ");
    std::string s5(" \r \n \t hello world \r \n \t ");
    RTrimString(&s1);
    RTrimString(&s2);
    RTrimString(&s3);
    RTrimString(&s4);
    RTrimString(&s5);
    ASSERT_EQ("hello world", s1);
    ASSERT_EQ("", s2);
    ASSERT_EQ(" hello world", s3);
    ASSERT_EQ("", s4);
    ASSERT_EQ(" \r \n \t hello world", s5);
  }
  {
    std::string s1("hello world");
    std::string s2("");
    std::string s3(" hello world ");
    std::string s4(" \r\t\n ");
    std::string s5(" \r \n \t hello world \r \n \t ");
    TrimString(&s1);
    TrimString(&s2);
    TrimString(&s3);
    TrimString(&s4);
    TrimString(&s5);
    ASSERT_EQ("hello world", s1);
    ASSERT_EQ("", s2);
    ASSERT_EQ("hello world", s3);
    ASSERT_EQ("", s4);
    ASSERT_EQ("hello world", s5);
  }
}

TEST_F(StringUtilityTest, TestStringReplace) {
  std::string send = "abbbccdbdbbbbdbd";
  StringReplace(&send, "bb", "b");
  ASSERT_EQ(send, "abccdbdbdbd");
  StringReplace(&send, "a", "a");
  ASSERT_EQ(send, "abccdbdbdbd");
  StringReplace(&send, "b", "a");
  ASSERT_EQ(send, "aaccdadadad");
}

TEST_F(StringUtilityTest, TestStringReplaceNonRec) {
  std::string send = "abbbccdbdbbbbdbd";
  StringReplaceNonRec(&send, "bb", "b");
  ASSERT_EQ(send, "abbccdbdbbdbd");
  StringReplaceNonRec(&send, "a", "a");
  ASSERT_EQ(send, "abbccdbdbbdbd");
  StringReplaceNonRec(&send, "b", "a");
  ASSERT_EQ(send, "aaaccdadaadad");
  StringReplaceNonRec(&send, "a", "aa");
  ASSERT_EQ(send, "aaaaaaccdaadaaaadaad");
}

TEST_F(StringUtilityTest, TestPopen) {
  std::string cmd = "ls -al";
  std::string result;
  ASSERT_TRUE(PopenToString(cmd.c_str(), &result));
  ASSERT_NE(result.find("build"), std::string::npos);
  ASSERT_TRUE(PopenFromString(cmd.c_str(), "echo ."));
}

TEST(StringPrintfTest, Empty) {
  EXPECT_EQ("", StringPrintf("%s", std::string().c_str()));
  EXPECT_EQ("", StringPrintf("%s", ""));
}

TEST(StringPrintfTest, Misc) {
  EXPECT_EQ("123hello w", StringPrintf(
                  "%3$d%2$s %1$c", 'w', "hello", 123));
}

TEST(StringAppendFTest, EmptyString) {
  std::string value("Hello");
  StringAppendF(&value, "%s", "");
  EXPECT_EQ("Hello", value);
}

TEST(StringAppendFTest, std_string) {
  std::string value("Hello");
  StringAppendF(&value, " %s", "World");
  EXPECT_EQ("Hello World", value);
}

TEST(StringAppendFTest, Int) {
  std::string value("Hello");
  StringAppendF(&value, " %d", 123);
  EXPECT_EQ("Hello 123", value);
}

TEST_F(StringUtilityTest, TestStringEqual) {
  std::string s1("test1");
  std::string s2("test");
  std::string s3("1test");
  std::string s4("test1");
  ASSERT_TRUE(StringEqual(s1, s4));
  ASSERT_FALSE(StringEqual(s1, s2));
  ASSERT_FALSE(StringEqual(s1, s3));
  ASSERT_FALSE(StringEqual(s2, s3));
}

TEST_F(StringUtilityTest, StringToLower) {
  std::string s("GoogleTencent");
  std::string s2("googletencent");
  StringToLower(&s);
  ASSERT_EQ(s2, s);
}

TEST_F(StringUtilityTest, StringExp) {
  std::vector<std::string> content;
  ASSERT_TRUE(StringExp("$PWD", &content));
  ASSERT_EQ(content.size(), 1u);
}

TEST_F(StringUtilityTest, StringJoin) {
  std::vector<std::string> field;
  field.push_back("hello1");
  field.push_back("hello2");
  field.push_back("hello3");
  field.push_back("hello4");

  ASSERT_EQ(JoinString(field, "&"), "hello1&hello2&hello3&hello4");
  ASSERT_EQ(JoinString(field.begin() + 2, field.end(), "|"), "hello3|hello4");
}

TEST_F(StringUtilityTest, StringMD5) {
  std::string md5 = MD5sumString("hello world");
  ASSERT_EQ(md5, "5eb63bbbe01eeed093cb22bb8f5acdc3");
}
