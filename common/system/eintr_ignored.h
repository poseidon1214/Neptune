// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 06/23/11
// Description: Define a helper macro to handle EINTR

#ifndef COMMON_SYSTEM_EINTR_IGNORED_H
#define COMMON_SYSTEM_EINTR_IGNORED_H
#pragma once

#include <error.h>

namespace gdt {

/// @define EINTR_IGNORED(expr)
/// Ignore EINTR for system call, for example,
/// EINTR_IGNORED(write(fd, buf, size)) will retry if EINTR occured

#ifdef __unix__
#define EINTR_IGNORED(expr) \
    ({ \
        __typeof__(expr) eintr_ignored_result; \
        do { \
            eintr_ignored_result = (expr); \
        } while (eintr_ignored_result < 0 && errno == EINTR); \
        eintr_ignored_result; \
    })
#else
#define EINTR_IGNORED(expr) (expr)
#endif

}
#endif // COMMON_SYSTEM_EINTR_IGNORED_H
