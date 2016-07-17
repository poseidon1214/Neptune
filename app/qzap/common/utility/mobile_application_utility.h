// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>

#ifndef APP_QZAP_COMMON_UTILITY_MOBILE_APPLICATION_UTILITY_H_
#define APP_QZAP_COMMON_UTILITY_MOBILE_APPLICATION_UTILITY_H_

#include <stdint.h>
#include <string>

uint32_t  GenerateMobileApplicationKeyId(const std::string& package_name,
                                         const std::string& version);

uint32_t  GenerateMobileApplicationPackageNameId(const std::string& package_name);
#endif  // APP_QZAP_COMMON_UTILITY_MOBILE_APPLICATION_UTILITY_H_
