// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/31/11

#include "common/system/concurrency/this_thread.h"

#include <string.h>

#if __unix__
#include <pthread.h>
#include <syscall.h>
#include <unistd.h>

namespace {

static bool SupportTls() __attribute__((pure));

#if defined(__linux__) && defined(__i386__) // All 64 bit machines are new.
// Slackware/2.4.30 kernel, using linuxthreads, doesn't really support __thread keyword,
// will generate runtime error
static bool SupportTls() {
  char buffer[64];
  confstr(_CS_GNU_LIBPTHREAD_VERSION, buffer, sizeof(buffer));
  return strstr(buffer, "NPTL") != NULL;
}
#else
static inline bool SupportTls() {
  return true;
}
#endif
// If symbol is weak, its address may be NULL. But the optimizer may always
// think it's non-NULL, so simply compare its address to NULL maybe not work.
// For __thread variable, in some unknown case, the address may also be NULL.
template <typename T>
static inline bool SymbolExists(const T& value) {
  const T* volatile address = &value;
  return address != NULL;
}

} // namespace

namespace gdt {

ThreadHandleType ThisThread::GetHandle() {
  return ::pthread_self();
}

int ThisThread::GetId() {
  static const bool support_tls = SupportTls();
  if (support_tls) {
    static __thread pid_t tid;
    if (!SymbolExists(tid))
      return syscall(SYS_gettid);
    if (tid == 0)
      tid = syscall(SYS_gettid);
    return tid;
  }
  return syscall(SYS_gettid);
}

void ThisThread::Exit() {
  pthread_exit(NULL);
}

void ThisThread::Sleep(int64_t time_in_ms) {
  if (time_in_ms >= 0) {
    timespec ts = { time_in_ms / 1000, (time_in_ms % 1000) * 1000000 };
    nanosleep(&ts, &ts);
  } else {
    pause();
  }
}

void ThisThread::Yield() {
  sched_yield();
}

int ThisThread::GetLastErrorCode() {
  return errno;
}

bool ThisThread::IsMain() {
  return ThisThread::GetId() == getpid();
}

} // namespace gdt

#endif // __unix__

