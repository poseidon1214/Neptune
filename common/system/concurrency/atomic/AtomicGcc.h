// Copyright (c) 2009, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCC_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCC_H

#if __i386__
#include "common/system/concurrency/atomic/AtomicGccX86.h"
#elif __x86_64__
#include "common/system/concurrency/atomic/AtomicGccX64.h"
#else
#error unsupported architect
#endif

#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCC_H

