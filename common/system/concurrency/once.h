// Copyright (c) 2011, Tencent Inc. All rights reserved.

/**
 * @file once.h
 * @brief
 * @author kypoyin,kypoyin@gmail.com
 * @date 2010-09-01
 */

#ifndef COMMON_SYSTEM_CONCURRENCY_ONCE_H
#define COMMON_SYSTEM_CONCURRENCY_ONCE_H

#include <pthread.h>

namespace gdt {

struct Once {
  pthread_once_t once;
  void Init(void (*init_routine_func)());
 private:
  // Once(const Once&) = delete;
  // Once operator=(const Once&) = delete;
};

#define DEFINE_ONCE(NAME)       Once NAME = { PTHREAD_ONCE_INIT }
#define DECLARE_ONCE(NAME) \
    DEPRECATED_MESSAGE("DECLARE_ONCE is deprecated, please use DEFINE_ONCE") \
    Once NAME = { PTHREAD_ONCE_INIT }

inline void Once::Init(void (*init_routine_func)()) {
  pthread_once(&once, init_routine_func);
}


} // namespace gdt

#endif // COMMON_SYSTEM_CONCURRENCY_ONCE_H
