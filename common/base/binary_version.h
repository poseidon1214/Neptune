// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/05/11
// Description: binary version information

#ifndef COMMON_BASE_BINARY_VERSION_H_
#define COMMON_BASE_BINARY_VERSION_H_
#pragma once

#include <string>

extern "C" {
namespace binary_version {
extern const int kSvnInfoCount;
extern const char * const kSvnInfo[];
extern const char kBuildType[];
extern const char kBuildTime[];
extern const char kBuilderName[];
extern const char kHostName[];
extern const char kCompiler[];
}
}

namespace common {

std::string MakeVersionInfo();

// Set version information to gflags, which make --version print usable
// text.
void InitVersionInfo();

}  // namespace common

#endif  // COMMON_BASE_BINARY_VERSION_H_
