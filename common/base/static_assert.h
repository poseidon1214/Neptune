// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_BASE_STATIC_ASSERT_H_
#define COMMON_BASE_STATIC_ASSERT_H_

#if __GXX_EXPERIMENTAL_CXX0X__ || defined __MSC_VER && __MSC_VER >= 1600
#define STATIC_ASSERT(e, ...) static_assert(e, "" __VA_ARGS__)
#else

#include "common/base/preprocess.h"

// namespace common {

template <bool x> struct static_assertion_failure;

template <> struct static_assertion_failure<true> {
    enum { value = 1 };
};

template<int x> struct static_assert_test {};

/// �����ڼ�ľ�̬����
/// @param e �������ʽ
/// ���ڱ����ڼ������ĳ��ϣ�ʾ����
/// @code
/// STATIC_ASSERT(sizeof(Foo) == 48, "Foo �Ĵ�С����Ϊ 48");
/// @endcode
#define STATIC_ASSERT(e, ...) \
typedef static_assert_test< \
            sizeof(static_assertion_failure<(bool)(e)>)> \
            PP_JOIN(static_assert_failed, __LINE__)

#endif

// } // namespace common

#endif  // COMMON_BASE_STATIC_ASSERT_H_

