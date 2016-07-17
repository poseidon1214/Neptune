// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Wed Aug  5 17:48:54 CST 2015

#ifndef COMMON_SYSTEM_PROC_STAT_H_
#define COMMON_SYSTEM_PROC_STAT_H_

#include <stdint.h>
#include <cstring>

namespace gdt {

// Get from /proc/stat
struct CpuJiffies {
  int64_t user;
  int64_t nice;
  int64_t system;
  int64_t idle;
  int64_t iowait;      // since Linux 2.5.41
  int64_t irq;         // since Linux 2.6.0
  int64_t softirq;     // since Linux 2.6.0
  int64_t steal;       // since Linux 2.6.1
  int64_t guest;       // since Linux 2.6.24
  int64_t guest_nice;  // since Linux 2.6.33
  CpuJiffies()
    : user(0),
      nice(0),
      system(0),
      idle(0),
      iowait(0),
      irq(0),
      softirq(0),
      steal(0),
      guest(0),
      guest_nice(0) {
  }
};

// Get from /proc/pid/stat
struct ProcessStat {
  int pid;                // 1
  char comm[128];         // 2
  char state;             // 3
  int ppid;               // 4
  int pgrp;               // 5
  int session;            // 6
  int tty_nr;             // 7
  int tpgid;              // 8
  uint32_t flags;         // 9
  uint64_t minflt;        // 10
  uint64_t cminflt;       // 11
  uint64_t majflt;        // 12
  uint64_t cmajflt;       // 13
  uint64_t utime;         // 14
  uint64_t stime;         // 15
  int64_t cutime;         // 16
  int64_t cstime;         // 17
  int64_t priority;       // 18
  int64_t nice;           // 19
  int64_t num_threads;    // 20
  int64_t itrealvalue;    // 21
  uint64_t starttime;     // 22
  uint64_t vsize;         // 23
  int64_t rss;            // 24
  uint64_t rsslim;        // 25
  uint64_t startcode;     // 26
  uint64_t endcode;       // 27
  uint64_t startstack;    // 28
  uint64_t kstkesp;       // 29
  uint64_t kstkeip;       // 30
  uint64_t signal;        // 31
  uint64_t blocked;       // 32
  uint64_t sigignore;     // 33
  uint64_t sigcatch;      // 34
  uint64_t wchan;         // 35
  uint64_t nswap;         // 36
  uint64_t cnswap;        // 37
  int exit_signal;        // 38
  int processor;          // 39
  uint32_t rt_priority;   // 40
  uint32_t policy;        // 41
  uint64_t delayacct_blkio_ticks;   // 42
  uint64_t guest_time;    // 43
  int64_t cguest_time;    // 44
  uint64_t start_data;    // 45
  uint64_t end_data;      // 46
  uint64_t start_brk;     // 47
  uint64_t arg_start;     // 48
  uint64_t arg_end;       // 49
  uint64_t env_start;     // 50
  uint64_t env_end;       // 51
  int exit_code;          // 52
  ProcessStat() {
    memset(this, 0, sizeof(ProcessStat));
  }
};

bool GetProcStat(CpuJiffies* cpu_jiffies);
bool GetProcPidStat(int pid, ProcessStat* process_stat);

// Sum of all kinds of cpu jiffies
// return -1 when failed
int64_t GetSumJiffies();
int64_t GetSumJiffies(const CpuJiffies& cpu_jiffies);

// utime + stime
int64_t GetSumPidJiffies(int pid);
int64_t GetSumPidJiffies(const ProcessStat& process_stat);

}  // namespace gdt

#endif  // COMMON_SYSTEM_PROC_STAT_H_
