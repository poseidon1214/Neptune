// Copyright (c) 2011, Tencent.com
// All rights reserved.

/// @file string_number_test.cpp
/// @brief string_number_test
/// @date  03/31/2011 10:42:51 AM
/// @author CHEN Feng <phongchen@tencent.com>

#include "common/base/string/string_number.h"
#include <math.h>
#include <algorithm>
#include <inttypes.h>

#include "thirdparty/gtest/gtest.h"

// namespace common {

template <typename T>
class StringNumberTest : public ::testing::Test { };
TYPED_TEST_CASE_P(StringNumberTest);

TYPED_TEST_P(StringNumberTest, ParseNumberInvalid) {
  const char* s = "hello";
  TypeParam n;
  EXPECT_FALSE(ParseNumber(s, &n));
  EXPECT_FALSE(ParseNumber(s, &n, NULL));

  char* endptr;
  EXPECT_FALSE(ParseNumber(s, &n, &endptr));
  EXPECT_EQ(s, endptr);
}

REGISTER_TYPED_TEST_CASE_P(StringNumberTest, ParseNumberInvalid);

typedef ::testing::Types<short, unsigned short, int, unsigned int,
        long, unsigned long, long long, unsigned long long, float,
        double, long double> NumericTypes;
INSTANTIATE_TYPED_TEST_CASE_P(NumericTypes, StringNumberTest, NumericTypes);

TEST(StringNumber, IsNaN) {
  float f = 1.000;
  ASSERT_FALSE(IsNaN(f));
  f = 1.223e+20;
  ASSERT_FALSE(IsNaN(f));
#ifdef __GNUC__
  f = INFINITY;
  ASSERT_FALSE(IsNaN(f));
#endif
  f = sqrt(-1.0);
  ASSERT_TRUE(IsNaN(f));
}


TEST(StringNumber, UIntToHexString) {
  EXPECT_EQ("9527", UInt16ToHexString(0x9527));
  EXPECT_EQ("95279527", UInt32ToHexString(0x95279527));
  EXPECT_EQ("9527952795279527", UInt64ToHexString(0x9527952795279527ULL));
}

// http://synesis.com.au/publications.html
const char* MatthewWilsonConvert(int value, char buf[]) {
  static char digits[19] = {
    '9', '8', '7', '6', '5', '4', '3', '2', '1',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
  };
  static const char* zero = digits + 9;  // zero 指向 '0'
  // works for -2147483648 .. 2147483647
  int i = value;
  char* p = buf;
  do {
    int lsd = i % 10;  // lsd 可能小于 0
    i /= 10;           // 是向下取整还是向零取整？
    *p++ = zero[lsd];  // 下标可能为负
  } while (i != 0);
  if (value < 0) {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);
  return p; // p - buf 即为整数长度
}

class StringNumberPerformanceTest : public testing::Test {
 protected:
  static const int kInt = 42576010;
  static const char kIntString[];
  static const int64_t kInt64 = 4257601042576010LL;
  static const char kInt64String[];
  static const double kDouble;
  static const char kDoubleString[];
 private:
  void SetUp() {
    int n;
    sscanf(kIntString, "%d", &n); // NOLINT(runtime/printf)
    ASSERT_EQ(kInt, n);
  }
};

const int StringNumberPerformanceTest::kInt;
const int64_t StringNumberPerformanceTest::kInt64;
const char StringNumberPerformanceTest::kIntString[] = "42576010";
const char StringNumberPerformanceTest::kInt64String[] = "4257601042576010";
const double StringNumberPerformanceTest::kDouble = 1110.32505;
const char StringNumberPerformanceTest::kDoubleString[] = "1110.32505";

TEST_F(StringNumberPerformanceTest, SScanfInt) {
  int n;
  for (int i = 0; i < 1000000; i++)
    sscanf(kIntString, "%d", &n); // NOLINT(runtime/printf)
}

TEST_F(StringNumberPerformanceTest, SScanfInt64) {
  int64_t n;
  for (int i = 0; i < 1000000; i++)
    sscanf(kInt64String, "%" SCNd64, &n); // NOLINT(runtime/printf)
}

TEST_F(StringNumberPerformanceTest, MatthewWilsonConvert) {
  char buffer[16];
  for (int i = 0; i < 1000000; i++)
    MatthewWilsonConvert(kInt + i, buffer);
}

TEST(StringNumber, FormatMeasure) {
  EXPECT_EQ("1", FormatMeasure(1));
  EXPECT_EQ("123", FormatMeasure(123));
  EXPECT_EQ("1.23k", FormatMeasure(1234));
  EXPECT_EQ("12.3k", FormatMeasure(12345));
  EXPECT_EQ("123k", FormatMeasure(123456));
  EXPECT_EQ("1.23M", FormatMeasure(1234567));
  EXPECT_EQ("12.3M", FormatMeasure(12345678));
  EXPECT_EQ("123M", FormatMeasure(123456789));

  EXPECT_EQ("1bps", FormatMeasure(1, "bps"));
  EXPECT_EQ("123bps", FormatMeasure(123, "bps"));
  EXPECT_EQ("1.23kbps", FormatMeasure(1234, "bps"));
  EXPECT_EQ("12.3kbps", FormatMeasure(12345, "bps"));
  EXPECT_EQ("123kbps", FormatMeasure(123456, "bps"));
  EXPECT_EQ("1.23Mbps", FormatMeasure(1234567, "bps"));
  EXPECT_EQ("12.3Mbps", FormatMeasure(12345678, "bps"));
  EXPECT_EQ("123Mbps", FormatMeasure(123456789, "bps"));
  EXPECT_EQ("0bps", FormatMeasure(0, "bps"));
  EXPECT_EQ("100 mF", FormatMeasure(0.1, " F"));
  EXPECT_EQ("12.3 mF", FormatMeasure(0.0123, " F"));
  EXPECT_EQ("1.23 mF", FormatMeasure(0.001234, " F"));
  EXPECT_EQ("123 uF", FormatMeasure(0.00012345, " F"));
  EXPECT_EQ("12.3 uF", FormatMeasure(0.0000123456, " F"));
  EXPECT_EQ("1.23 uF", FormatMeasure(0.000001234567, " F"));
  EXPECT_EQ("123 nF", FormatMeasure(0.00000012345678, " F"));
  EXPECT_EQ("12.3 nF", FormatMeasure(0.0000000123456789, " F"));
}

TEST(StringNumber, PhysicalConstantsMeasure) {
  EXPECT_EQ("300 Mm/s", FormatMeasure(299792458, " m/s"));
  EXPECT_EQ("160 zC", FormatMeasure(1.60217733e-19, " C"));
  EXPECT_EQ("6.63e-34 J.s", FormatMeasure(6.6260755e-34, " J.s"));
  EXPECT_EQ("2.82 fm", FormatMeasure(2.81794092e-15, " m"));
  EXPECT_EQ("13.8 yJ/K", FormatMeasure(1.380658e-23, " J/K"));
}

TEST(StringNumber, FormatBinaryMeasure) {
  EXPECT_EQ("1 B/s", FormatBinaryMeasure(1, " B/s"));
  EXPECT_EQ("123 B/s", FormatBinaryMeasure(123, " B/s"));
  EXPECT_EQ("1023 B/s", FormatBinaryMeasure(1023, " B/s"));
  EXPECT_EQ("1.21 KiB/s", FormatBinaryMeasure(1234, " B/s"));
  EXPECT_EQ("12.1 KiB/s", FormatBinaryMeasure(12345, " B/s"));
  EXPECT_EQ("121 KiB/s", FormatBinaryMeasure(123456, " B/s"));
  EXPECT_EQ("1.18 MiB/s", FormatBinaryMeasure(1234567, " B/s"));
  EXPECT_EQ("11.8 MiB/s", FormatBinaryMeasure(12345678, " B/s"));
  EXPECT_EQ("118 MiB/s", FormatBinaryMeasure(123456789, " B/s"));
  EXPECT_EQ("1.15 GiB/s", FormatBinaryMeasure(1234567890, " B/s"));
  EXPECT_EQ("11.5 GiB/s", FormatBinaryMeasure(12345678900ULL, " B/s"));
  EXPECT_EQ("115 GiB/s", FormatBinaryMeasure(123456789000ULL, " B/s"));
  EXPECT_EQ("1.12 TiB/s", FormatBinaryMeasure(1234567890000ULL, " B/s"));
}

TEST(StringNumber, MeasureUnderflow) {
  EXPECT_EQ("1y", FormatMeasure(1e-24));
  EXPECT_EQ("1e-25", FormatMeasure(1e-25));
  EXPECT_EQ("1e-100", FormatMeasure(1e-100));
}

TEST(StringNumber, MeasureOverflow) {
  EXPECT_EQ("1Y", FormatMeasure(1e24));
  EXPECT_EQ("100Y", FormatMeasure(1e26));
  EXPECT_EQ("1e+27", FormatMeasure(1e27));
  EXPECT_EQ("1e+100", FormatMeasure(1e100));
}

TEST(StringNumber, MeasureNanAndInf) {
  EXPECT_EQ("inf", FormatMeasure(INFINITY));
  EXPECT_EQ("inf US$", FormatMeasure(INFINITY, " US$"));
  EXPECT_EQ("infUS$", FormatMeasure(INFINITY, "US$"));
  EXPECT_EQ("nan", FormatMeasure(NAN));
  EXPECT_EQ("nan X", FormatMeasure(NAN, " X"));
}

TEST(StringNumber, IntegralNumber) {
  signed char c = 0;
  EXPECT_TRUE(ParseNumber("88", &c, NULL, 0));
  EXPECT_FALSE(ParseNumber("129", &c, NULL, 0));
  EXPECT_TRUE(ParseNumber("-33", &c, NULL, 0));
  unsigned char d = 0;
  EXPECT_TRUE(ParseNumber("129", &d, NULL, 0));
  EXPECT_FALSE(ParseNumber("266", &d, NULL, 0));
  EXPECT_FALSE(ParseNumber("-33", &d, NULL, 0));
  short e = 0;
  EXPECT_TRUE(ParseNumber("266", &e, NULL, 0));
  EXPECT_FALSE(ParseNumber("32777", &e, NULL, 0));
  EXPECT_TRUE(ParseNumber("-33", &e, NULL, 0));
  EXPECT_FALSE(ParseNumber("-32777", &e, NULL, 0));
  unsigned short f = 0;
  EXPECT_TRUE(ParseNumber("32777", &f, NULL, 0));
  EXPECT_FALSE(ParseNumber("66666", &f, NULL, 0));
  EXPECT_FALSE(ParseNumber("-33", &f, NULL, 0));
  int g = 0;
  EXPECT_TRUE(ParseNumber("32777", &g, NULL, 0));
  EXPECT_FALSE(ParseNumber("2147483666", &g, NULL, 0));
  EXPECT_TRUE(ParseNumber("-33", &g, NULL, 0));
  unsigned int h = 0;
  EXPECT_TRUE(ParseNumber("2147483666", &h, NULL, 0));
  EXPECT_FALSE(ParseNumber("4294967299", &h, NULL, 0));
  EXPECT_FALSE(ParseNumber("-33", &h, NULL, 0));
  long i = 0;
#if defined __x86_64__
  EXPECT_TRUE(ParseNumber("4294967299", &i, NULL, 0));
  EXPECT_TRUE(ParseNumber("-2147483666", &i, NULL, 0));
#else
  EXPECT_FALSE(ParseNumber("4294967299", &i, NULL, 0));
  EXPECT_FALSE(ParseNumber("-2147483666", &i, NULL, 0));
#endif
  unsigned long j = 0;
  EXPECT_FALSE(ParseNumber("-2147483", &j, NULL, 0));
  long long k = 0;
  EXPECT_TRUE(ParseNumber("4294967299", &k, NULL, 0));
  EXPECT_FALSE(ParseNumber("9223372036854775808", &k, NULL, 0));
  EXPECT_TRUE(ParseNumber("-2147483666", &k, NULL, 0));
  unsigned long long l = 0;
  EXPECT_TRUE(ParseNumber("9223372036854775808", &l, NULL, 0));
  EXPECT_FALSE(ParseNumber("18446744073709551616", &l, NULL, 0));
  EXPECT_FALSE(ParseNumber("-2147483666", &l, NULL, 0));
}

TEST(StringNumber, FixedWidthNumber) {
  int32_t i = 0;
  EXPECT_TRUE(ParseNumber("32777", &i, NULL, 0));
  EXPECT_FALSE(ParseNumber("2147483666", &i, NULL, 0));
  EXPECT_TRUE(ParseNumber("-33", &i, NULL, 0));
  EXPECT_FALSE(ParseNumber("-2147483649", &i, NULL, 10));
  uint32_t j = 0;
  EXPECT_TRUE(ParseNumber("2147483666", &j, NULL, 0));
  EXPECT_FALSE(ParseNumber("4294967299", &j, NULL, 0));
  EXPECT_FALSE(ParseNumber("-33", &j, NULL, 0));
  EXPECT_FALSE(ParseNumber("-2222222222222222147483649", &j, NULL, 10));
  int64_t k = 0;
  EXPECT_TRUE(ParseNumber("4294967299", &k, NULL, 0));
  EXPECT_FALSE(ParseNumber("9223372036854775808", &k, NULL, 0));
  EXPECT_TRUE(ParseNumber("-2147483666", &k, NULL, 0));
  EXPECT_FALSE(ParseNumber("-9223372036854775809", &k, NULL, 0));
  uint64_t l = 0;
  EXPECT_TRUE(ParseNumber("9223372036854775808", &l, NULL, 0));
  EXPECT_FALSE(ParseNumber("18446744073709551616", &l, NULL, 0));
  EXPECT_FALSE(ParseNumber("-2147483666", &l, NULL, 0));
  EXPECT_FALSE(ParseNumber("-2222222222222222147483649", &l, NULL, 10));
}

TEST(StringNumber, IsHexNumericString) {
  std::string s1 = "12345";
  ASSERT_TRUE(IsHexNumericString(s1));
  std::string s2 = "1234567890abcdef";
  ASSERT_TRUE(IsHexNumericString(s2));
  std::string s3 = "123abcxx";
  ASSERT_FALSE(IsHexNumericString(s3));
}

// } // namespace common
