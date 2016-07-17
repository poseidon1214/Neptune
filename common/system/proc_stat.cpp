// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Wed Aug  5 17:48:51 CST 2015

#include "common/system/proc_stat.h"

#include <inttypes.h>
#include <stdio.h>
#include "thirdparty/glog/logging.h"

namespace gdt {

bool GetProcStat(CpuJiffies* cpu_jiffies) {
  FILE* file = fopen("/proc/stat", "r");
  if (file == NULL) {
    LOG(ERROR) << "Fail to open '/proc/stat', " << strerror(errno);
    return false;
  }

  char buf[128];
  if (!fgets(buf, sizeof(buf), file)) {
    LOG(ERROR) << "Fail to fgets '/proc/stat', " << strerror(errno);
    fclose(file);
    return false;
  }
  fclose(file);

  char cpu[5];
  int n = sscanf(buf, "%s"
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64,
                 cpu,
                 &cpu_jiffies->user,
                 &cpu_jiffies->nice,
                 &cpu_jiffies->system,
                 &cpu_jiffies->idle,
                 &cpu_jiffies->iowait,
                 &cpu_jiffies->irq,
                 &cpu_jiffies->softirq,
                 &cpu_jiffies->steal,
                 &cpu_jiffies->guest,
                 &cpu_jiffies->guest_nice);
  const int kMinColInProcStat = 5;
  if (n < kMinColInProcStat) {
    LOG(ERROR) << "Error format, '" << buf << "'";
    return false;
  }

  return true;
}

bool GetProcPidStat(int pid, ProcessStat* process_stat) {
  char filename[128];
  snprintf(filename, sizeof(filename), "/proc/%d/stat", pid);
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    LOG(ERROR) << "Fail to open '" << filename << "', " << strerror(errno);
    return false;
  }

  char buf[1024];
  if (!fgets(buf, sizeof(buf), file)) {
    LOG(ERROR) << "Fail to fgets '" << filename << "', " << strerror(errno);
    fclose(file);
    return false;
  }
  fclose(file);

  int n = sscanf(buf, "%d"
                      "%s "
                      "%c"
                      "%d"
                      "%d"
                      "%d"
                      "%d"
                      "%d"
                      "%u"
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNd64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNd64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%d"
                      "%d"
                      "%u"
                      "%u"
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNd64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%" SCNu64
                      "%d",
                 &process_stat->pid,
                 process_stat->comm,
                 &process_stat->state,
                 &process_stat->ppid,
                 &process_stat->pgrp,
                 &process_stat->session,
                 &process_stat->tty_nr,
                 &process_stat->tpgid,
                 &process_stat->flags,
                 &process_stat->minflt,
                 &process_stat->cminflt,
                 &process_stat->majflt,
                 &process_stat->cmajflt,
                 &process_stat->utime,
                 &process_stat->stime,
                 &process_stat->cutime,
                 &process_stat->cstime,
                 &process_stat->priority,
                 &process_stat->nice,
                 &process_stat->num_threads,
                 &process_stat->itrealvalue,
                 &process_stat->starttime,
                 &process_stat->vsize,
                 &process_stat->rss,
                 &process_stat->rsslim,
                 &process_stat->startcode,
                 &process_stat->endcode,
                 &process_stat->startstack,
                 &process_stat->kstkesp,
                 &process_stat->kstkeip,
                 &process_stat->signal,
                 &process_stat->blocked,
                 &process_stat->sigignore,
                 &process_stat->sigcatch,
                 &process_stat->wchan,
                 &process_stat->nswap,
                 &process_stat->cnswap,
                 &process_stat->exit_signal,
                 &process_stat->processor,
                 &process_stat->rt_priority,
                 &process_stat->policy,
                 &process_stat->delayacct_blkio_ticks,
                 &process_stat->guest_time,
                 &process_stat->cguest_time,
                 &process_stat->start_data,
                 &process_stat->end_data,
                 &process_stat->start_brk,
                 &process_stat->arg_start,
                 &process_stat->arg_end,
                 &process_stat->env_start,
                 &process_stat->env_end,
                 &process_stat->exit_code);
  const int kMinColInProcPidStat = 20;
  if (n < kMinColInProcPidStat) {
    LOG(ERROR) << "Error format, n: "<< n << ", '" << buf << "'";
    return false;
  }

  return true;
}

int64_t GetSumJiffies() {
  CpuJiffies jiffies;
  if (!GetProcStat(&jiffies)) {
    return -1;
  }
  return GetSumJiffies(jiffies);
}

int64_t GetSumJiffies(const CpuJiffies& jiffies) {
  return jiffies.user + jiffies.nice + jiffies.system + jiffies.idle +
         jiffies.iowait + jiffies.irq + jiffies.softirq + jiffies.steal +
         jiffies.guest + jiffies.guest_nice;
}

int64_t GetSumPidJiffies(int pid) {
  ProcessStat process_stat;
  if (!GetProcPidStat(pid, &process_stat)) {
    return -1;
  }
  return GetSumPidJiffies(process_stat);
}

int64_t GetSumPidJiffies(const ProcessStat& process_stat) {
  return process_stat.utime + process_stat.stime;
}

}  // namespace gdt
