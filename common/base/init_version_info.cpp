// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2013-05-09

#include <string>
#include <sstream>
#include "common/base/binary_version.h"
#include "thirdparty/gflags/gflags.h"

namespace common {

std::string MakeVersionInfo() {
  using namespace binary_version;

  std::ostringstream oss;
  oss << "\n";  // Open a new line in gflags --version output.

  oss << "BuildTime: " << kBuildTime << "\n"
      << "BuildType: " << kBuildType << "\n"
      << "BuilderName: " << kBuilderName << "\n"
      << "HostName: " << kHostName << "\n"
      << "Compiler: " << kCompiler << "\n";

  if (kSvnInfoCount > 0) {
    std::string line_breaker(100, '-');  // ----------
    oss << "Sources:\n" << line_breaker << "\n";
    for (int i = 0; i < kSvnInfoCount; ++i)
      oss << kSvnInfo[i];
    oss << line_breaker << "\n";
  }

  return oss.str();
}

void InitVersionInfo() {
  google::SetVersionString(MakeVersionInfo());
}

}  // namespace common

