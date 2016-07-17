// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_BASE_PREPROCESS_STRINGIZE_H_
#define COMMON_BASE_PREPROCESS_STRINGIZE_H_
#pragma once

/// Converts the parameter X to a string after macro replacement
/// on X has been performed.
/// example: PP_STRINGIZE(UCHAR_MAX) -> "255"
#define PP_STRINGIZE(X) PP_DO_STRINGIZE(X)
#define PP_DO_STRINGIZE(X) #X

#endif  // COMMON_BASE_PREPROCESS_STRINGIZE_H_
