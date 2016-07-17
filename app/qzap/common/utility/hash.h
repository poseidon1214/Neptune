// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5
#ifndef APP_QZAP_COMMON_UTILITY_HASH_H_
#define APP_QZAP_COMMON_UTILITY_HASH_H_

#include <stdint.h>
#include <functional>
#include <string>
#ifdef __cplusplus
extern "C" {
#endif
extern uint64_t hash3(const uint8_t*, uint64_t, uint64_t);
static inline uint64_t hash_string(const char *s, size_t size) {
  return hash3((const uint8_t*)(s),
               static_cast<uint64_t>(size), 0xbeef);
}

static inline uint64_t hash_data(const uint8_t*s, uint64_t size, uint64_t key) {
  return hash3(s, size, key);
}

#ifdef __cplusplus
}
#endif
static inline uint64_t hash_string(const std::string &x) {
  return hash_string(x.c_str(), x.size());
}
#endif  // APP_QZAP_COMMON_UTILITY_HASH_H_
