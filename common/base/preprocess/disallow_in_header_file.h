// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_BASE_PREPROCESS_DISALLOW_IN_HEADER_FILE_H_
#define COMMON_BASE_PREPROCESS_DISALLOW_IN_HEADER_FILE_H_
#pragma once

#include "common/base/static_assert.h"

/// disallow macro be used in header files
///
/// @example
/// #define SOMEMACRO() PP_DISALLOW_IN_HEADER_FILE()
/// A compile error will be issued if SOMEMACRO() is used in header files
#ifdef __GNUC__
# define PP_DISALLOW_IN_HEADER_FILE() \
    STATIC_ASSERT(__INCLUDE_LEVEL__ == 0, \
                  "This macro can not be used in header files");
#else
# define PP_DISALLOW_IN_HEADER_FILE()
#endif

#endif  // COMMON_BASE_PREPROCESS_DISALLOW_IN_HEADER_FILE_H_
