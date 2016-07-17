// Copyright 2011, Tencent Inc.
// Author: Yongsong Liu <ericliu@tencent.com>

#ifndef COMMON_NET_HTTP_HTTP_TIME_H
#define COMMON_NET_HTTP_HTTP_TIME_H

#include <time.h>
#include <string>

namespace gdt {

bool ParseHttpTime(const char* str, time_t* time);
inline bool ParseHttpTime(const std::string& str, time_t* time) {
  return ParseHttpTime(str.c_str(), time);
}

size_t FormatHttpTime(time_t time, char* str, size_t str_length);
bool FormatHttpTime(time_t time, std::string* str);

} // namespace gdt

#endif // COMMON_NET_HTTP_HTTP_TIME_H
