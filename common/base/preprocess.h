// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_BASE_PREPROCESS_H_
#define COMMON_BASE_PREPROCESS_H_

#include "common/base/preprocess/disallow_in_header_file.h"
#include "common/base/preprocess/join.h"
#include "common/base/preprocess/stringize.h"
#include "common/base/preprocess/varargs.h"

/// prevent macro substitution for function-like macros
/// if macro 'min()' was defined:
/// 'int min()' whill be substituted, but
/// 'int min PP_PREVENT_MACRO_SUBSTITUTION()' will not be substituted.
#define PP_PREVENT_MACRO_SUBSTITUTION

#endif  // COMMON_BASE_PREPROCESS_H_

