// Copyright (c) 2011, Tencent Inc. All rights reserved.
/// @brief Convert string to number and number to string
/// @author hsiaokangliu
/// @date 2010-11-25

#ifndef COMMON_BASE_STRING_STRING_NUMBER_H_
#define COMMON_BASE_STRING_STRING_NUMBER_H_

#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <string>
#include <tr1/type_traits>

#include "common/base/static_assert.h"
#include "common/encoding/ascii.h"

// GLOBAL_NOLINT(runtime/int)

// windows header defined these macros, kill them!
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

// namespace common {

/// -----------------------------------------------------------------------
/// @brief Parse the string from the first position. stop when error occurs.
/// @return true if part of the string can be converted to a valid number
/// @return false if the number exceeds limit or nothing is parsed.
/// @param str the string to parse
/// @param value store the parsed number if success
/// @param endptr not null, *endptr stores the address of the first invalid char
/// !!! If no invalid char is allowed, use function below: StringToNumber()
/// @param base specify the base to be used for the conversion, 0 means automatic  // NOLINT
/// -----------------------------------------------------------------------

bool ParseNumber(const char* str, signed char* value, char** endptr = NULL,
                 int base = 0);
bool ParseNumber(const char* str, unsigned char* value, char** endptr = NULL,
                 int base = 0);
bool ParseNumber(const char* str, short* value, char** endptr = NULL,
                 int base = 0);
bool ParseNumber(const char* str, unsigned short* value, char** endptr = NULL,
                 int base = 0);
bool ParseNumber(const char* str, int* value, char** endptr = NULL,
                 int base = 0);
bool ParseNumber(const char* str, unsigned int* value, char** endptr = NULL,
                 int base = 0);
bool ParseNumber(const char* str, long* value, char** endptr = NULL,
                 int base = 0);
bool ParseNumber(const char* str, unsigned long* value, char** endptr = NULL,
                 int base = 0);
bool ParseNumber(const char* str, long long* value, char** endptr = NULL,
                 int base = 0);
bool ParseNumber(const char* str, unsigned long long* value,
                 char** endptr = NULL, int base = 0);
bool ParseNumber(const char* str, float* value, char** endptr = NULL);
bool ParseNumber(const char* str, double* value, char** endptr = NULL);
bool ParseNumber(const char* str, long double* value, char** endptr = NULL);

/// ---------------------------------------------------------------
/// @brief interface for parsing string to number
/// ---------------------------------------------------------------
template <typename Type>
bool ParseNumber(const std::string& str, Type* value, char** endptr, int base) {
  return ParseNumber(str.c_str(), value, endptr, base);
}

template <typename Type>
bool ParseNumber(const std::string& str, Type* value, char** endptr) {
  return ParseNumber(str.c_str(), value, endptr);
}

template <typename T>
bool StringToNumeric(const std::string &s, T *val, int32_t base = 0);

template<>
bool StringToNumeric(const std::string &s, int16_t *val, int32_t base);
template<>
bool StringToNumeric(const std::string &s, int32_t *val, int32_t base);
template<>
bool StringToNumeric(const std::string &s, int64_t *val, int32_t base);
template<>
bool StringToNumeric(const std::string &s, uint16_t *val, int32_t base);
template<>
bool StringToNumeric(const std::string &s, uint32_t *val, int32_t base);
template<>
bool StringToNumeric(const std::string &s, uint64_t *val, int32_t base);
template<>
bool StringToNumeric(const std::string &s, float *val, int32_t base);
template<>
bool StringToNumeric(const std::string &s, double *val, int32_t base);


/// ---------------------------------------------------------------
/// @brief converting numbers  to buffer, buffer size should be big enough
/// ---------------------------------------------------------------
const int kMaxIntegerStringSize = 32;
const int kMaxDoubleStringSize = 32;
const int kMaxFloatStringSize = 24;

/// @brief judge a number if it's nan
inline bool IsNaN(double value) {
  // cppcheck-suppress duplicateExpression
  return !(value > value) && !(value <= value);
}

template <typename T>
const std::string ConvertToString(const T &val);

template<> const std::string ConvertToString(const int32_t &val);
template<> const std::string ConvertToString(const int64_t &val);
template<> const std::string ConvertToString(const uint32_t &val);
template<> const std::string ConvertToString(const uint64_t &val);
template<> const std::string ConvertToString(const double &val);
template<> inline const std::string ConvertToString(const std::string &val) {
  return val;
}
inline const std::string ConvertToString(const char *val) {
  return std::string(val);
}

/// @brief write number to buffer as string
/// @return end of result
/// @note without '\0' appended
/// private functions for common library, don't use them in your code
char* WriteDoubleToBuffer(double n, char* buffer);
char* WriteFloatToBuffer(float n, char* buffer);
char* WriteInt32ToBuffer(int32_t i, char* buffer);
char* WriteUInt32ToBuffer(uint32_t u, char* buffer);
char* WriteInt64ToBuffer(int64_t i, char* buffer);
char* WriteUInt64ToBuffer(uint64_t u64, char* buffer);

char* WriteIntegerToBuffer(int n, char* buffer);
char* WriteIntegerToBuffer(unsigned int n, char* buffer);
char* WriteIntegerToBuffer(long n, char* buffer);
char* WriteIntegerToBuffer(unsigned long n, char* buffer);
char* WriteIntegerToBuffer(long long n, char* buffer);
char* WriteIntegerToBuffer(unsigned long long n, char* buffer);

void AppendIntegerToString(int n, std::string* str);
void AppendIntegerToString(unsigned int n, std::string* str);
void AppendIntegerToString(long n, std::string* str);
void AppendIntegerToString(unsigned long n, std::string* str);
void AppendIntegerToString(long long n, std::string* str);
void AppendIntegerToString(unsigned long long n, std::string* str);

/// @brief convert number to hex string
/// buffer size should be at least [2 * sizeof(value) + 1]
char* WriteHexUInt16ToBuffer(uint16_t value, char* buffer);
char* WriteHexUInt32ToBuffer(uint32_t value, char* buffer);
char* WriteHexUInt64ToBuffer(uint64_t value, char* buffer);

char* UInt16ToHexString(uint16_t value, char* buffer);
char* UInt32ToHexString(uint32_t value, char* buffer);
char* UInt64ToHexString(uint64_t value, char* buffer);

/// @brief convert number to hex string, not so efficient but more convenient
std::string UInt16ToHexString(uint16_t n);
std::string UInt32ToHexString(uint32_t n);
std::string UInt64ToHexString(uint64_t n);

///////////////////////////////////////////////////////////////////////////////
// convert number to human readable string

/// Convert decimal number to human readable string, based on 1000.
/// @param n the number to be converted
/// @param unit the unit of the number, such as "m/s", "Hz", etc.
///             the unit can have an optional space(" ") prefix,
///             such as " bps", and then 10000 will be convert to "10 kbps"
/// @note this function support float number and negative number, keep 3
/// significant digits.
std::string FormatMeasure(double n, const char* unit = "");

/// Convert number to human readable string, based on 1024.
/// @param n the number to be converted
/// @param unit the unit of the number, such as "m/s"
///             the unit can have an optional space(" ") prefix,
///             such as " B", and then 10240 will be convert to "10 KiB"
/// this function support only integral value, keep 3 significant digits.
std::string FormatBinaryMeasure(int64_t n, const char* unit = "");

/// @brief check if a string has only hex digit, nothing else.
bool IsHexNumericString(const std::string &s);

// } // namespace common

#endif  // COMMON_BASE_STRING_STRING_NUMBER_H_
