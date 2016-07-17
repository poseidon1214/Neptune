
// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 06/23/11
// Description:

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/system/check_error.h"

namespace gdt {
void HandleErrnoError(const char* function_name, int error) {
  const char* msg = strerror(error);
  fprintf(stderr, "%s: Fatal error, %s", function_name, msg);
  abort();
}
void HandlePosixError(const char* function_name) {
  HandleErrnoError(function_name, errno);
}

bool HandlePosixTimedError(const char* function_name) {
  int error = errno;
  if (error == ETIMEDOUT)
    return false;
  HandleErrnoError(function_name, error);
  return true;
}

bool HandlePthreadTimedError(const char* function_name, int error) {
  if (error == ETIMEDOUT)
    return false;
  HandleErrnoError(function_name, error);
  return false;
}

bool HandlePthreadTryLockError(const char* function_name, int error) {
  if (error == EBUSY || error == EAGAIN)
    return false;
  HandleErrnoError(function_name, error);
  return false;
}

}
