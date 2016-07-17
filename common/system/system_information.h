// Copyright (c) 2010, Tencent Inc. All rights reserved.
// Author: Chen Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_SYSTEM_INFORMATION_H_
#define COMMON_SYSTEM_SYSTEM_INFORMATION_H_

// GLOBAL_NOLINT(runtime/int)

#include <string>

// namespace common {

unsigned int GetLogicalCpuNumber();
unsigned long long GetPhysicalMemorySize();
unsigned long long GetTotalPhysicalMemorySize();
bool GetOsKernelInfo(std::string* kernerl_info);
bool GetMachineArchitecture(std::string* arch_info);
std::string GetUserName();
bool GetMemoryUsage(double* usage);

// } // namespace common

#endif  // COMMON_SYSTEM_SYSTEM_INFORMATION_H_
