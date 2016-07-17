// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include "data_collector/feeder/common/rules_common.h"
#include "app/qzap/common/utility/time_utility.h"
#include "app/qzap/common/base/string_utility.h"
#include "common/base/string/string_number.h"

namespace gdt {
namespace rules {

void StringToUpper(std::string* str) {
  for (size_t i = 0; i < str->size(); ++i) {
    str->at(i) = std::toupper(str->at(i));
  }
}

std::string GenerateFilename(const std::string& url,
	                         const std::string& data_dir) {
  return data_dir + "/" + ConvertToString(MD5(url)) + ".xml";
}

std::string GetUnixTime(const std::string& day) {
  std::string str = day;
  ReplaceAll(&str, "-", "");
  return str.substr(0, std::min(size_t(8), str.size()));
}

uint64_t ConvertToProductId(std::string outer_id,
                            uint64_t source_id) {
  return (source_id << 32) + (ConvertToOuterId(outer_id) & 0xFFFFFFFF);
}

uint64_t MD5(const std::string& str) {
  std::string md5_str = MD5sumString(str);
  uint64_t md5;
  std::stringstream ss;
  ss << md5_str.substr(md5_str.size() - 16);
  ss >> std::hex >> md5;
  return md5;
}

uint64_t ConvertToOuterId(std::string outer_str) {
  uint64_t outer_id = 0;
  if (!StringToNumeric(outer_str, &outer_id)) {
    StringToUpper(&outer_str);
    return MD5(outer_str);
  }
  return outer_id;
}

}  // namespace rules
}  // namespace gdt
