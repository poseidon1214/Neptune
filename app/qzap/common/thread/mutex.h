// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-1-17
#ifndef APP_QZAP_COMMON_THREAD_MUTEX_H_
#define APP_QZAP_COMMON_THREAD_MUTEX_H_

// #warning Please include "common/system/concurrency/mutex.h"

#include "app/qzap/common/base/base.h"
#include "common/system/concurrency/condition_variable.h"
#include "common/system/concurrency/mutex.h"

typedef gdt::Mutex Mutex;  // __attribute__((deprecated));
typedef gdt::MutexLocker MutexLock;  // __attribute__((deprecated));

#endif  // APP_QZAP_COMMON_THREAD_MUTEX_H_
