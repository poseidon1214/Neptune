// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2010-06-21

#include "common/system/system_information.h"
#include <stdlib.h>

// GLOBAL_NOLINT(runtime/int)

#ifdef __unix__

#include <pwd.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/user.h>
#include <sys/utsname.h>

#elif defined _WIN32
#include <common/base/common_windows.h>

#endif
// namespace common {

#ifdef __unix__
unsigned int GetLogicalCpuNumber() {
  return sysconf(_SC_NPROCESSORS_ONLN);
}

unsigned long long GetPhysicalMemorySize() {
  return sysconf(_SC_PHYS_PAGES) * (unsigned long long) PAGE_SIZE;
}

static const struct utsname* GetUnameInfo() {
  static struct utsname uts;
  if (uname(&uts) == -1)
    return NULL;

  return &uts;
}

bool GetOsKernelInfo(std::string* kernerl_info) {
  const struct utsname* uts = GetUnameInfo();
  if (uts != NULL) {
    kernerl_info->assign(uts->release);
    return true;
  }

  return false;
}

bool GetMachineArchitecture(std::string* arch_info) {
  const struct utsname* uts = GetUnameInfo();
  if (uts != NULL) {
    arch_info->assign(uts->machine);
    return true;
  }

  return false;
}

std::string GetUserName() {
  char buf[1024];
  struct passwd pwd;
  struct passwd* ppwd = NULL;
  if (getpwuid_r(getuid(), &pwd, buf, sizeof(buf), &ppwd) != 0 &&
      ppwd != NULL) {
    const char* username = getenv("USER");
    return username != NULL ? username : getenv("USERNAME");
  }
  return ppwd->pw_name;
}

bool GetMemoryUsage(double* usage) {
  struct sysinfo info;
  int ret = sysinfo(&info);
  if (ret != 0) {
    return false;
  }

  int64_t total_ram = info.totalram + info.totalswap;
  int64_t free_ram = info.freeram + info.freeswap;
  *usage = (total_ram - free_ram) / static_cast<double>(total_ram);
  return true;
}

#elif defined _WIN32

unsigned int GetLogicalCpuNumber() {
  SYSTEM_INFO SystemInfo;
  GetSystemInfo(&SystemInfo);
  return SystemInfo.dwNumberOfProcessors;
}

unsigned long long GetPhysicalMemorySize() {
  MEMORYSTATUSEX memory_status = { sizeof(memory_status) };
  GlobalMemoryStatusEx(&memory_status);
  return memory_status.ullTotalPhys;
}

bool GetOsKernelInfo(std::string* kernerl_info) {
  return false;
}

bool GetMachineArchitecture(std::string* arch_info) {
  return false;
}

std::string GetUserName() {
  const char* username = getenv("USER");
  return username != NULL ? username : getenv("USERNAME");
}

#endif

unsigned long long GetTotalPhysicalMemorySize() {
  return GetPhysicalMemorySize();
}

// } // namespace common
