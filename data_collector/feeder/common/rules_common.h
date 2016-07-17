// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)


#ifndef DATA_COLLECTOR_FEEDER_COMMON_RULES_COMMON_H_
#define DATA_COLLECTOR_FEEDER_COMMON_RULES_COMMON_H_

#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <istream>
#include <sstream>
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "common/file/file_tools.h"
#include "common/base/string/algorithm.h"

namespace gdt {
namespace rules {

void StringToUpper(std::string * str);

uint64_t MD5(const std::string& str);

std::string GenerateFilename(const std::string& url, const std::string& data_dir = "");

std::string GetUnixTime(const std::string& day);

uint64_t ConvertToProductId(std::string outer_id,
                            uint64_t source_id);

uint64_t ConvertToOuterId(std::string outer_id);


};
}  // namespace gdt

#endif  // DATA_COLLECTOR_FEEDER_COMMON_RULES_COMMON_H_
