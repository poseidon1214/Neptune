// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
// This file includes routines to find out characteristics
// of the machine a program is running on.  It is undoubtedly
// system-dependent.

// Functions listed here that accept a pid_t as an argument act on the
// current process if the pid_t argument is 0

#ifndef PLATFORM_BASE_SYSINFO_H_
#define PLATFORM_BASE_SYSINFO_H_

#include <stdint.h>
#include <list>
#include <string>

// processor cycles per second of each processor
double System_CyclesPerSecond(void);

// Number of logical processors (hyperthreads) in system. See
int System_NumCPUs(void);

// Return total CPU used by this process, including child threads.
// Unlike ProcessCPUUsage(getpid()), the value returned by
// MyCPUUsage() includes the CPU usage of all threads created within
// this process.
double MyCPUUsage();

void WaitATick();
void SchedYield();

// System_Ip retrieve the ip of the net(lo, eth0, ...)
bool System_Ip(const char *net, std::string *ip);

//System_Ip("eth1", &ip);
std::string IntranetIp();
bool ConvertIpToNumeric(const std::string& ip_str, int32_t* ip);

// Mount entry in the file system.
struct MountEntry {
  std::string mount;
  std::string device;
  uint32_t total;  // k byte
  uint32_t free;   // k byte
};
bool System_FileSystem(std::list<MountEntry> *fs_info);

// k byte
bool System_DiskTotal(uint64_t *total);

// k byte
bool System_DiskFree(uint64_t *free);

// Get idx cpu usage ratio
void GetCpuRatio(int32_t idx, int32_t span, double* ratio);

// Get user name
const char* UserName();
#endif  // PLATFORM_BASE_SYSINFO_H_
