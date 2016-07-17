// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 06/23/11
// Description: common pthread error handler

#ifndef COMMON_SYSTEM_CHECK_ERROR_H
#define COMMON_SYSTEM_CHECK_ERROR_H

#pragma once
namespace gdt {

void HandleErrnoError(const char* function_name, int error);
inline void CheckErrnoError(const char* function_name, int error) {
  if (error)
    HandleErrnoError(function_name, error);
}

void HandlePosixError(const char* function_name);
inline void CheckPosixError(const char* function_name, int result) {
  if (result < 0)
    HandlePosixError(function_name);
}

bool HandlePosixTimedError(const char* function_name);
inline bool CheckPosixTimedError(const char* function_name, int result) {
  if (result < 0)
    return HandlePosixTimedError(function_name);
  return true;
}

bool HandlePthreadTimedError(const char* function_name, int error);
inline bool CheckPthreadTimedError(const char* function_name, int error) {
  if (error)
    return HandlePthreadTimedError(function_name, error);
  return true;
}

bool HandlePthreadTryLockError(const char* function_name, int error);
inline bool CheckPthreadTryLockError(const char* function_name, int error) {
  if (error)
    return HandlePthreadTryLockError(function_name, error);
  return true;
}

#define CHECK_ERRNO_ERROR(expr) \
    CheckErrnoError(__PRETTY_FUNCTION__, (expr))

#define CHECK_POSIX_ERROR(expr) \
    CheckPosixError(__PRETTY_FUNCTION__, (expr))

#define CHECK_POSIX_TIMED_ERROR(expr) \
    CheckPosixTimedError(__PRETTY_FUNCTION__, (expr))

#define CHECK_PTHREAD_ERROR(expr) \
    CHECK_ERRNO_ERROR((expr))

#define CHECK_PTHREAD_TIMED_ERROR(expr) \
    CheckPthreadTimedError(__PRETTY_FUNCTION__, (expr))

#define CHECK_PTHREAD_TRYLOCK_ERROR(expr) \
    CheckPthreadTryLockError(__PRETTY_FUNCTION__, (expr))

}
#endif // COMMON_SYSTEM_CHECK_ERROR_H
