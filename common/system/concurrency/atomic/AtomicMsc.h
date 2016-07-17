// Copyright (c) 2009, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSC_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSC_H

#ifdef _M_IX86
#include "common/system/concurrency/atomic/AtomicMscX86.h"
#else
#include "common/system/concurrency/atomic/AtomicMscIntrinsic.h"
typedef AtomicMscIntrinsic AtomicImplementation;
#endif

#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSC_H

