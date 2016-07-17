// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>

#include "app/qzap/common/utility/mobile_application_utility.h"

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/hash.h"

uint32_t  GenerateMobileApplicationKeyId(const std::string& package_name,
                                         const std::string& version) {
  return hash_string(package_name + "_" + version);
}

uint32_t  GenerateMobileApplicationPackageNameId(const std::string& package_name) {
  return hash_string(package_name);
}
