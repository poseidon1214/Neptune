// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/base/gflags_in_spp.h"
#include <errno.h>
#include <string>
#include <vector>
#include "common/base/string/algorithm.h"
#include "thirdparty/gflags/gflags.h"

namespace gdt {

void InitGflagsInSpp(const void* etc) {
  // Allow mutiple flag files
  std::vector<std::string> flag_files;
  SplitStringAndStrip(static_cast<const char*>(etc), " ", &flag_files);
  google::AllowCommandLineReparsing();

  for (size_t i = 0; i < flag_files.size(); ++i) {
    google::ReadFromFlagsFile(flag_files[i], program_invocation_name, true);
  }
}

}  // namespace gdt
