// Copyright (c) 2015 Tencent Inc.

#ifndef COMMON_BASE_STRING_UTILITY_H_
#define COMMON_BASE_STRING_UTILITY_H_

#include <stdint.h>

#include <string>
#include <vector>

bool PopenToString(const char *command, std::string *result);
bool PopenFromString(const char *command, const char *input);

uint32_t ChecksumString(const std::string& str);

// the api of md5sum
std::string MD5sumString(const std::string &str);

// exp a string like shell.
bool StringExp(const char *exp, std::vector<std::string> *result);

#endif  // COMMON_BASE_STRING_UTILITY_H_
