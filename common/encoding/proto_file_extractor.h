// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Mon Sep 14 20:32:52 CST 2015

#ifndef COMMON_ENCODING_PROTO_FILE_EXTRACTOR_H_
#define COMMON_ENCODING_PROTO_FILE_EXTRACTOR_H_

#include <map>
#include <string>
#include "thirdparty/glog/logging.h"
#include "thirdparty/protobuf/descriptor.pb.h"

namespace gdt {

class ProtoFileExtractor {
 public:
  enum Type {
    PACKAGE = 0,
    DEPENDENCY,
    MESSAGE,
    ENUM,
    ENUMVALUE,
    SERVICE,
    FIELD,
    METHOD,
    EXTENSION,

    TYPE_COUNT,
  };

  struct Comments {
    Comments(const std::string& l, const std::string& t)
        : leading_comments(l), trailing_comments(t) {}
    Comments() {}
    std::string leading_comments;
    std::string trailing_comments;
  };
  typedef std::map<std::string, Comments> Map;

  // input: FileDescriptorProto
  // output: Map[TYPE_COUNT]
  static bool Parse(const google::protobuf::FileDescriptorProto& proto,
                    Map (*maps)[TYPE_COUNT]);

 private:
  typedef google::protobuf::int32 pb_int32;
  typedef google::protobuf::RepeatedField<pb_int32> Int32FieldVector;
  typedef google::protobuf::FileDescriptorProto FileDescriptorProto;
  typedef google::protobuf::DescriptorProto DescriptorProto;
  typedef google::protobuf::EnumDescriptorProto EnumDescriptorProto;
  typedef google::protobuf::EnumValueDescriptorProto EnumValueDescriptorProto;
  typedef google::protobuf::ServiceDescriptorProto ServiceDescriptorProto;
  typedef google::protobuf::FieldDescriptorProto FieldDescriptorProto;
  typedef google::protobuf::DescriptorProto::ExtensionRange ExtensionRange;
  typedef google::protobuf::MethodDescriptorProto MethodDescriptorProto;

  static bool TracePathFile(const FileDescriptorProto& proto,
                            const Int32FieldVector& path,
                            Type* type, std::string* full_name);
  static bool TracePathMessage(const DescriptorProto& proto,
                               const Int32FieldVector& path, int idx,
                               Type* type, std::string* full_name);
  static bool TracePathEnum(const EnumDescriptorProto& proto,
                            const Int32FieldVector& path, int idx,
                            Type* type, std::string* full_name);
  static bool TracePathEnumValue(const EnumValueDescriptorProto& proto,
                                 const Int32FieldVector& path, int idx,
                                 Type* type, std::string* full_name);
  static bool TracePathService(const ServiceDescriptorProto& proto,
                               const Int32FieldVector& path, int idx,
                               Type* type, std::string* full_name);
  static bool TracePathField(const FieldDescriptorProto& proto,
                             const Int32FieldVector& path, int idx,
                             Type* type, std::string* full_name);
  static bool TracePathMethod(const MethodDescriptorProto& proto,
                              const Int32FieldVector& path, int idx,
                              Type* type, std::string* full_name);
  static bool TracePathExtension(const ExtensionRange& proto,
                                 const Int32FieldVector& path, int idx,
                                 Type* type, std::string* full_name);
};

}  // namespace gdt

#endif  // COMMON_ENCODING_PROTO_FILE_EXTRACTOR_H_
