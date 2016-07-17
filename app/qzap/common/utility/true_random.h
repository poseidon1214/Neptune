// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef APP_QZAP_COMMON_UTILITY_TRUE_RANDOM_H_
#define APP_QZAP_COMMON_UTILITY_TRUE_RANDOM_H_

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "common/base/uncopyable.h"

namespace gdt {

/// true random generator
class TrueRandom : Uncopyable {
 public:
  TrueRandom();
  ~TrueRandom();

  /// return random integer in range [0, UINT_MAX]
  uint32_t NextUInt32();

  /// return random integer in range [0, max_value)
  uint32_t NextUInt32(uint32_t max_value);

  /// return double in range [0.0,1.0]
  double NextDouble();

  /// generate random bytes
  bool NextBytes(void* buffer, size_t size);

 private:
  int m_fd;               /// fd for /dev/urandom
};

}  // namespace gdt

#endif  // APP_QZAP_COMMON_UTILITY_TRUE_RANDOM_H_
