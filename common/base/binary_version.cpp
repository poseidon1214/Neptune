// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/23/11
// Description:

// Don't include header here, otherwise gcc 4.6.2+ will report:
// warning: 'weakref' attribute ignored because variable is initialized
// #include "common/base/binary_version.h"

#include "common/base/binary_version.h"

extern "C" {
namespace binary_version {
__attribute__((weak)) extern const int kSvnInfoCount = 0;
__attribute__((weak)) extern const char * const kSvnInfo[] = {0};
__attribute__((weak)) extern const char kBuildType[] = "Unknown";
__attribute__((weak)) extern const char kBuildTime[] = "Unknown";
__attribute__((weak)) extern const char kBuilderName[] = "Unknown";
__attribute__((weak)) extern const char kHostName[] = "Unknown";
__attribute__((weak)) extern const char kCompiler[] = "Unknown";
}  // namespace binary_version
}  // extern "C"


