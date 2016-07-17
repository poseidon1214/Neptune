// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 2013-02-28

#ifndef COMMON_BASE_TYPE_TRAITS_H_
#define COMMON_BASE_TYPE_TRAITS_H_
#pragma once

#include <features.h>

#if __GNUC_PREREQ(4, 1)
#define COMMON_HAS_STD_TR1_TYPE_TRAITS
#endif

#if __GNUC_PREREQ(4, 5) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define COMMON_HAS_STD_TYPE_TRAITS
#endif

#if defined COMMON_HAS_STD_TYPE_TRAITS
#include "common/base/type_traits/std.h"
#elif defined COMMON_HAS_STD_TR1_TYPE_TRAITS
#include "common/base/type_traits/tr1.h"
#else
#include "common/base/type_traits/missing.h"
#endif

#undef COMMON_HAS_STD_TYPE_TRAITS
#undef COMMON_HAS_STD_TR1_TYPE_TRAITS

#endif  // COMMON_BASE_TYPE_TRAITS_H_
