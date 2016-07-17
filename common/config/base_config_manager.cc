// Copyright (c) 2014 Tencent Inc.
// Author: Li Meng (elvisli@tencent.com)

#include "common/config/base_config_manager.h"

#include "app/qzap/common/utility/file_utility.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/protobuf/text_format.h"

namespace gdt {

bool BaseConfigManager_LoadAndParse(const std::string& file_path,
                                    google::protobuf::Message* message) {
  std::string content;
  if (!ReadFileToString(file_path, &content)) {
    LOG(ERROR) << "Failed to load config file " << file_path;
    return false;
  }
  if (!google::protobuf::TextFormat::ParseFromString(content, message)) {
    LOG(ERROR) << "Failed to parse config file " << file_path;
    return false;
  }
  return true;
}

}  // namespace gdt

