// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-2-21
// Typically, these routines will all be os, and possibly processor,
// specific.  Every routine should thus be protected by ifdefs so
// that programs won't compile if these routines are run on a
// processor/OS that haven't been supported yet.
#include <arpa/inet.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <mntent.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/vfs.h>
#include <unistd.h>
#include <algorithm>
#include <sstream>
#include <vector>
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/sysinfo.h"

// ----------------------------------------------------------------------
// CyclesPerSecond()
//   To convert from cycles to seconds, we compute CyclesPerSecond,
//   if some other CycleTimer hasn't already done so.  If we can't
//   figure out the count, we set it to 1.  We try to get the
//   number of cycles from /proc/cpuinfo (at least on Linux).
// ----------------------------------------------------------------------

static pthread_once_t cpuinfo_init = PTHREAD_ONCE_INIT;
static double cpuinfo_cycles_per_second = 1.0;  // 0.0 might be dangerous
static int cpuinfo_num_cpus = 1;  // Conservative guess
static bool cpuinfo_support_sse = false;
static bool cpuinfo_support_sse2 = false;
#define USERNAME_LEN 20
static char user_name[USERNAME_LEN];

static void GetUserName() {
  const char *user = getenv("USER");
  if (user != NULL) {
    size_t len = strlen(user);
    CHECK_LT(len, static_cast<size_t>(USERNAME_LEN));
    strncpy(user_name, user, len);
  } else {
    strncpy(user_name, "invalid-user", strlen("invalid-user"));
  }
}

// WARNING: logging calls back to InitializeSystemInfo so it must not invoke
// any logging code.
static void InitializeSystemInfo() {
  // If CPU scaling is in effect, we want to use the *maximum* frequency,
  // not whatever CPU speed some random processor happens to be using now.
  bool saw_mhz = false;
  const char* pname0 = "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq";
  GetUserName();
  FILE* f0 = fopen(pname0, "r");
  if (f0 != NULL) {
    int max_freq;
    if (fscanf(f0, "%d", &max_freq) == 1) {
      // The value is in kHz.  For example, on a 2GHz warpstation, the
      // file contains the value "2000" (with no newline or anything).
      cpuinfo_cycles_per_second = max_freq * 1000.0;
      saw_mhz = true;
    }
    fclose(f0);
  }

  // Read /proc/cpuinfo for other values, and if there is no cpuinfo_max_freq.
  const char* pname = "/proc/cpuinfo";
  FILE* f = fopen(pname, "r");
  CHECK_NE(f, reinterpret_cast<FILE*>(NULL));

  char line[1024];
  double frequency = 1.0;
  double bogo_clock = 1.0;
  int cpu_id;
  int max_cpu_id = 0;
  int sse_count = 0;
  int sse2_count = 0;
  int num_cpus = 0;
  std::stringstream s;
  std::string dummy_str1, dummy_str2, dummy_str3;
  while ( fgets(line, sizeof(line), f) ) {
    line[sizeof(line)-1] = '\0';

    if (!saw_mhz && strstr(line, "cpu MHz") != NULL) {
      s.str("");  // donot forget to clear
      s << line;
      // format is "cpu MHz : %lf"
      s >> dummy_str1 >> dummy_str2 >> dummy_str3 >> frequency;
      cpuinfo_cycles_per_second = frequency * 1000000.0;
      saw_mhz = true;
    } else if (strstr(line, "bogomips") != NULL) {
      s.str("");  // donot forget to clear
      s << line;
      // format is "bogomips : %lf"
      s >> dummy_str1 >> dummy_str2 >> frequency;
      bogo_clock = frequency * 1000000.0;
    } else if (strstr(line, "processor") != NULL) {
      s.str("");  // donot forget to clear
      s << line;
      // format is "processor : %d"
      s >> dummy_str1 >> dummy_str2 >> cpu_id;
      num_cpus++;  // count up every time we see an "processor :" entry
      max_cpu_id = std::max(max_cpu_id, cpu_id);
    }

    // check for sse{2,3}, occurs on the same line
    // verify that all cpus have the same info
    // sse, sse2, sse3 always appear in that order
    // sse2 means sse is also available, sse3 means that
    // sse2 is also available
    const char* sse = strstr(line, " sse");
    const char* sse2 = strstr(line, " sse2");
    const char* sse3 = strstr(line, " sse3");
    if (sse || sse2 || sse3) {
      cpuinfo_support_sse = true;
      sse_count++;
    }
    if (sse2 || sse3) {
      cpuinfo_support_sse2 = true;
      sse2_count++;
    }
  }
  fclose(f);
  if (!saw_mhz) {
    // If we didn't find anything better, we'll use bogomips, but
    // we're not happy about it.
    cpuinfo_cycles_per_second = bogo_clock;
  }
  if ((sse_count > 0) && (sse_count != num_cpus)) {
    fprintf(stderr, "Only %d of %d cpus have SSE, disabling SSE\n",
            sse_count, num_cpus);
    cpuinfo_support_sse = false;
  }
  if ((sse2_count > 0) && (sse2_count != num_cpus)) {
    fprintf(stderr, "Only %d of %d cpus have SSE2, disabing SSE2\n",
            sse2_count, num_cpus);
    cpuinfo_support_sse2 = false;
  }

  if (num_cpus == 0) {
    fprintf(stderr, "Failed to read num. CPUs correctly from /proc/cpuinfo\n");
  } else {
    if ((max_cpu_id + 1) != num_cpus) {
      fprintf(stderr,
              "CPU ID assignments in /proc/cpuinfo seems messed up."
              " This is usually caused by a bad BIOS.\n");
    }
    cpuinfo_num_cpus = num_cpus;
  }
}

const char* UserName() {
  pthread_once(&cpuinfo_init, &InitializeSystemInfo);
  return user_name;
}

double System_CyclesPerSecond(void) {
  pthread_once(&cpuinfo_init, &InitializeSystemInfo);
  return cpuinfo_cycles_per_second;
}

int System_NumCPUs(void) {
  pthread_once(&cpuinfo_init, &InitializeSystemInfo);
  return cpuinfo_num_cpus;
}

bool SupportSSE(void) {
  pthread_once(&cpuinfo_init, &InitializeSystemInfo);
  return cpuinfo_support_sse;
}

bool SupportSSE2(void) {
  pthread_once(&cpuinfo_init, &InitializeSystemInfo);
  return cpuinfo_support_sse2;
}

double MyCPUUsage() {
  struct rusage ru;
  if (getrusage(RUSAGE_SELF, &ru) == 0) {
    return (static_cast<double>(ru.ru_utime.tv_sec)      +
            static_cast<double>(ru.ru_utime.tv_usec)/1e6 +
            static_cast<double>(ru.ru_stime.tv_sec)      +
            static_cast<double>(ru.ru_stime.tv_usec)/1e6);
  } else {
    return 0.0;
  }
}

void WaitATick() {
  int save_errno = errno;
  struct timespec tm;
  tm.tv_sec = 0;
  tm.tv_nsec = 2000001;
  nanosleep(&tm, NULL);
  errno = save_errno;
}

void SchedYield() {
  int save_errno = errno;
  sched_yield();
  errno = save_errno;
}

std::string IntranetIp()
{
    std::string ip;
    return System_Ip("eth1", &ip) ? ip : "";
}
bool ConvertIpToNumeric(const std::string& ip_str, int32_t* ip)
{
    if (ip_str.empty() || ip == NULL)
    {
        return false;
    }
    in_addr addr;
    if(!inet_aton(ip_str.c_str(), &addr))
    {
        LOG(ERROR) << "Invaid Ip" << ip;
        return false;
    }
    *ip = addr.s_addr;
    return true;
}
bool System_Ip(const char *net, std::string *ip) {
  if (net == NULL || ip == NULL) {
    return false;
  }
  struct ifreq ifr;
  strncpy(ifr.ifr_name, net, IFNAMSIZ);
  int fd = -1;
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return false;
  }
  int ret = ioctl(fd, SIOCGIFADDR, &ifr);
  close(fd);
  if (ret < 0) {
    return false;
  }
  struct sockaddr_in myaddr;
  memset(&myaddr, 0, sizeof(myaddr));
  memcpy(&myaddr, &ifr.ifr_addr, sizeof(myaddr));

  *ip = inet_ntoa(myaddr.sin_addr);
  return true;
}

static uint64_t kscale(uint64_t b, uint64_t bs) {
  return (b * bs + 1024/2) / 1024;
}

bool System_FileSystem(std::list<MountEntry> *fs_info) {
  FILE *mount_table;
  mount_table = setmntent("/etc/mtab", "r");
  if (!mount_table) {
    return false;
  }
  struct mntent *mount_entry;
  MountEntry entry;
  struct statfs fs;
  while (true) {
    mount_entry = getmntent(mount_table);
    if (!mount_entry) {
      endmntent(mount_table);
      break;
    }
    entry.mount = mount_entry->mnt_dir;
    entry.device = mount_entry->mnt_fsname;
    if (statfs(mount_entry->mnt_dir, &fs) != 0) {
      return false;
    }
    entry.total = kscale(fs.f_blocks, fs.f_bsize);
    entry.free = kscale(fs.f_bfree, fs.f_bsize);
    fs_info->push_back(entry);
  }
  return fs_info->size() > 0;
}

bool System_DiskTotal(uint64_t *total) {
  if (total == NULL) {
    return false;
  }
  std::list<MountEntry> fs_info;
  if (!System_FileSystem(&fs_info)) {
    return false;
  }
  *total = 0;
  for (std::list<MountEntry>::iterator it = fs_info.begin();
      it != fs_info.end(); ++it) {
    *total += it->total;
  }
  return true;
}

bool System_DiskFree(uint64_t *free) {
  if (free == NULL) {
    return false;
  }
  std::list<MountEntry> fs_info;
  if (!System_FileSystem(&fs_info)) {
    return false;
  }
  *free = 0;
  for (std::list<MountEntry>::iterator it = fs_info.begin();
      it != fs_info.end(); ++it) {
    *free += it->free;
  }
  return true;
}

static void GetCpuData(int32_t idx, int64_t *idle, int64_t *total) {
  // std::string cpu = std::string_utility::std::stringPrintf("cpu%d", idx);
  char cpu[16] = {0};
  snprintf(cpu, sizeof(cpu), "cpu%d", idx);
  FILE *fp = fopen("/proc/stat", "r");
  CHECK(fp != NULL);
  char buf[512] = { 0 };
  while (!feof(fp)) {
    memset(buf, 0x00, sizeof(buf));
    fgets(buf, sizeof(buf)-1, fp);
    if (strncmp(buf, cpu, strlen(cpu)) == 0) {
      break;
    }
  }
  fclose(fp);
  // cpu_i user nice system idle iowait irq softirq xx xx
  std::vector<int64_t> vi;
  char *ptr = NULL;
  char *ptr_tmp = NULL;
  ptr = strtok_r(buf, " ", &ptr_tmp);
  while (ptr) {
    vi.push_back(atol(ptr));
    ptr = strtok_r(NULL, " ", &ptr_tmp);
  }
  CHECK_GE(static_cast<int>(vi.size()), 8);
  while (vi.size() != 8) vi.pop_back();
  *idle = vi[4];
  std::vector<int64_t>::const_iterator it;
  for (it = vi.begin(); it != vi.end(); ++it) {
    *total += (*it);
  }
}

void GetCpuRatio(int32_t idx, int32_t span, double* ratio) {
  int64_t idle1 = 0, idle2 = 0;
  int64_t total1 = 0, total2 = 0;
  GetCpuData(idx, &idle1, &total1);
  sleep(span);
  GetCpuData(idx, &idle2, &total2);
  *ratio = 100.0 - (idle2-idle1)*1.0/(total2-total1)*100.0;
}
