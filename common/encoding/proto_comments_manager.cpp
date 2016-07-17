// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Tue Sep 15 16:07:23 CST 2015

#include "common/encoding/proto_comments_manager.h"

#include <string>
#include "common/system/file.h"

namespace gdt {

ProtoCommentsManager::ProtoCommentsManager() {
}

ProtoCommentsManager::ProtoCommentsManager(
    const google::protobuf::FileDescriptorSet& set) {
  Merge(set);
}

ProtoCommentsManager::~ProtoCommentsManager() {
}

bool ProtoCommentsManager::Merge(
    const google::protobuf::FileDescriptorSet& set) {

  ProtoFileExtractor::Type types[] = {
    ProtoFileExtractor::MESSAGE,
    ProtoFileExtractor::ENUM,
    ProtoFileExtractor::ENUMVALUE,
    ProtoFileExtractor::SERVICE,
    ProtoFileExtractor::FIELD,
    ProtoFileExtractor::METHOD
  };
  size_t type_size = sizeof(types) / sizeof(ProtoFileExtractor::Type);
  for (size_t i = 0; i < set.file_size(); ++i) {
    Map maps[ProtoFileExtractor::TYPE_COUNT];
    if (!ProtoFileExtractor::Parse(set.file(i), &maps)) {
      LOG(ERROR) << "Fail to parse " << i << "th file";
      return false;
    }

    for (size_t j = 0; j < type_size; ++j) {
      //  map_.insert(begin, end);
      //  log warning
      Map::const_iterator it = maps[types[j]].begin();
      for (; it != maps[types[j]].end(); ++it) {
        if (!(map_.insert(*it).second)) {
          LOG(WARNING) << "Fail to insert '" << it->first << "'";
          continue;
        }
      }
    }
  }
  return true;
}

bool ProtoCommentsManager::MergeFromFile(const std::string& filename) {
  std::string data;
  if (!file::ReadAll(filename, &data)) {
    LOG(ERROR) << "Fail to read file '" << filename << "'";
    return false;
  }

  google::protobuf::FileDescriptorSet set;
  if (!set.ParseFromString(data)) {
    LOG(ERROR) << "Fail to parse '" << filename << "'";
    return false;
  }

  return Merge(set);
}

bool ProtoCommentsManager::Find(
    const std::string& full_name,
    std::string* leading_comments,
    std::string* trailing_comments) const {
  Map::const_iterator it = map_.find(full_name);
  if (it == map_.end()) {
    return false;
  }

  *leading_comments = it->second.leading_comments;
  *trailing_comments = it->second.trailing_comments;
  return true;
}
}  // namespace gdt
