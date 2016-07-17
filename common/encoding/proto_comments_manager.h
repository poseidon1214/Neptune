// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Tue Sep 15 16:07:21 CST 2015

#ifndef COMMON_ENCODING_PROTO_COMMENTS_MANAGER_H_
#define COMMON_ENCODING_PROTO_COMMENTS_MANAGER_H_

#include <map>
#include <string>

#include "common/encoding/proto_file_extractor.h"
#include "google/protobuf/descriptor.pb.h"

namespace gdt {

class ProtoCommentsManager {
 public:
  ProtoCommentsManager();
  explicit ProtoCommentsManager(const google::protobuf::FileDescriptorSet& set);
  virtual ~ProtoCommentsManager();

  virtual bool Merge(const google::protobuf::FileDescriptorSet& set);
  virtual bool MergeFromFile(const std::string& filename);
  virtual bool Find(const std::string& full_name,
                    std::string* leading_comments,
                    std::string* trailing_comments) const;
 private:
  typedef ProtoFileExtractor::Comments Comments;
  typedef std::map<std::string, Comments> Map;

  Map map_;
};
}  // namespace gdt

#endif  // COMMON_ENCODING_PROTO_COMMENTS_MANAGER_H_
