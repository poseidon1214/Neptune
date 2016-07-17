/**
 *   Copyright (C) RainVan(Yunfeng.Xiao). All rights reserved.

 *   \file     mod_setaffinity.c
 *   \author   RainVan(Yunfeng.Xiao)
 *   \date     Jan 2013
 *   \version  1.0.0
 *   \brief    Implementation of Setting CPU affinity of Process (Linux)
 */
#include <sched.h>
#include <unistd.h>
#include <errno.h>
#include "app/qzap/common/utility/mod_setaffinity.h"

int mod_setaffinity(int pid, uint64_t mask)
{
    cpu_set_t cpus;
    int n = 0;

    CPU_ZERO(&cpus);

    for ( ; mask; ++n, mask >>= 1)
    {
        if (mask & 1)
        {
            CPU_SET(n, &cpus);
        }
    }

    /* set affinity of pid */
    return sched_setaffinity(pid, sizeof(cpu_set_t), &cpus);
}

int mod_bindaffinity(int pid, int id)
{
    cpu_set_t cpus;

    if (id < 0 || id >= sysconf(_SC_NPROCESSORS_CONF))
    {
        errno = EINVAL;
        return -1;
    }

    CPU_ZERO(&cpus);
    CPU_SET(id, &cpus);

    /* set affinity of pid */
    return sched_setaffinity(pid, sizeof(cpu_set_t), &cpus);
}

