// Copyright (c) 1998-2015 Tencent Technology Inc. All rights reserved.
// Author: foreveryang
// Create: Mon Sep 14 20:32:54 CST 2015

#include "common/encoding/proto_file_extractor.h"

#include <cstdio>
#include <string>

#include "common/base/string/concat.h"
#include "thirdparty/protobuf/text_format.h"

namespace gdt {

static std::string IntVectorToString(
    const google::protobuf::RepeatedField<google::protobuf::int32>& path) {
  typedef google::protobuf::int32 int32;
  typedef google::protobuf::TextFormat::FieldValuePrinter FieldValuePrinter;
  typedef google::protobuf::RepeatedField<int32>::const_iterator const_iterator;

  FieldValuePrinter printer;
  std::string p;
  const_iterator it = path.begin();
  for (; it != path.end(); ++it) {
    if (it != path.begin()) {
      p += "." + printer.PrintInt32(*it);
    } else {
      p = "[" + printer.PrintInt32(*it);
    }
  }
  p += "]";
  return p;
}

static void AppendFullName(std::string* full_name, const std::string& append) {
  if (full_name->empty()) {
    *full_name = append;
  } else {
    *full_name += "." + append;
  }
}

bool ProtoFileExtractor::Parse(
    const google::protobuf::FileDescriptorProto& proto,
    Map (*maps)[TYPE_COUNT]) {
  typedef google::protobuf::SourceCodeInfo::Location Location;
  if (!proto.has_source_code_info()) {
    LOG(ERROR) << "Fail to find source code info";
    return false;
  }

  for (size_t i = 0; i < proto.source_code_info().location_size(); ++i) {
    const Location& location = proto.source_code_info().location(i);
    if (!location.has_leading_comments() && !location.has_trailing_comments()) {
      continue;
    }

    Type type;
    std::string full_name;
    if (!TracePathFile(proto, location.path(), &type, &full_name)) {
      LOG(WARNING) << "Path " << IntVectorToString(location.path())
                   << " is not supported, ignore";
      continue;
    }

    if (proto.has_package() &&
        (type == MESSAGE || type == ENUM || type == ENUMVALUE
         || type == SERVICE || type == FIELD || type == METHOD
         || type == EXTENSION)) {
      full_name = proto.package() + "." + full_name;
    }

    (*maps)[type][full_name] = Comments(location.leading_comments(),
                                      location.trailing_comments());
  }
  return true;
}

bool ProtoFileExtractor::TracePathFile(
    const FileDescriptorProto& proto,
    const Int32FieldVector& path,
    Type* type, std::string* full_name) {

  pb_int32 field_index = path.Get(0);
  int value_index = (path.size() == 1) ? 0 : path.Get(1);
  switch (field_index) {
    // package
    case 2:
      *full_name = proto.package();
      *type = PACKAGE;
      return true;
    // dependency
    case 3:
      *full_name = proto.dependency(value_index);
      *type = DEPENDENCY;
      return true;
    case 4:
      return TracePathMessage(proto.message_type(value_index),
                              path, 2, type, full_name);
    case 5:
      return TracePathEnum(proto.enum_type(value_index),
                           path, 2, type, full_name);
    case 6:
      return TracePathService(proto.service(value_index),
                              path, 2, type, full_name);
    case 7:
      return TracePathField(proto.extension(value_index),
                            path, 2, type, full_name);
    case 8:
      // FileOptions
      return false;
    default:
      return false;
  }
}

bool ProtoFileExtractor::TracePathMessage(
    const DescriptorProto& proto,
    const Int32FieldVector& path, int idx,
    Type* type, std::string* full_name) {
  AppendFullName(full_name, proto.name());
  if (idx >= path.size()) {
    *type = MESSAGE;
    return true;
  }
  pb_int32 field_index = path.Get(idx);
  int value_index = (idx + 1 == path.size()) ? 0 : path.Get(idx + 1);
  switch (field_index) {
    case 2:
      return TracePathField(proto.field(value_index),
                            path, idx + 2, type, full_name);
    case 6:
      return TracePathField(proto.extension(value_index),
                            path, idx + 2, type, full_name);
    case 3:
      return TracePathMessage(proto.nested_type(value_index),
                              path, idx + 2, type, full_name);
    case 4:
      return TracePathEnum(proto.enum_type(value_index),
                           path, idx + 2, type, full_name);
    case 5:
      // extension name is 'extension_${idx}'
      StringAppend(full_name, ".extension_", value_index);
      *type = EXTENSION;
      return true;
    case 8:
      // Oneof
      return false;
    case 7:
      // MessageOptions
      return false;
    default:
      return false;
  }
}

bool ProtoFileExtractor::TracePathEnum(
    const EnumDescriptorProto& proto,
    const Int32FieldVector& path, int idx,
    Type* type, std::string* full_name) {
  AppendFullName(full_name, proto.name());
  if (idx >= path.size()) {
    *type = ENUM;
    return true;
  }
  pb_int32 field_index = path.Get(idx);
  int value_index = (idx + 1 == path.size()) ? 0 : path.Get(idx + 1);
  switch (field_index) {
    case 2:
      return TracePathEnumValue(proto.value(value_index),
                                path, idx + 2, type, full_name);
  }
  return false;
}

bool ProtoFileExtractor::TracePathEnumValue(
    const EnumValueDescriptorProto& proto,
    const Int32FieldVector& path, int idx,
    Type* type, std::string* full_name) {
  AppendFullName(full_name, proto.name());
  if (idx >= path.size()) {
    *type = ENUMVALUE;
    return true;
  }
  return false;
}

bool ProtoFileExtractor::TracePathService(
    const ServiceDescriptorProto& proto,
    const Int32FieldVector& path, int idx,
    Type* type, std::string* full_name) {
  AppendFullName(full_name, proto.name());
  if (idx >= path.size()) {
    *type = SERVICE;
    return true;
  }
  pb_int32 field_index = path.Get(idx);
  int value_index = (idx + 1 == path.size()) ? 0 : path.Get(idx + 1);
  switch (field_index) {
    case 2:
      return TracePathMethod(proto.method(value_index),
                             path, idx + 2, type, full_name);
    default:
      return false;
  }
}

bool ProtoFileExtractor::TracePathMethod(
    const MethodDescriptorProto& proto,
    const Int32FieldVector& path, int idx,
    Type* type, std::string* full_name) {
  AppendFullName(full_name, proto.name());
  if (idx >= path.size()) {
    *type = METHOD;
    return true;
  }
  return false;
}

bool ProtoFileExtractor::TracePathField(
    const FieldDescriptorProto& proto,
    const Int32FieldVector& path, int idx,
    Type* type, std::string* full_name) {
  AppendFullName(full_name, proto.name());
  if (idx >= path.size()) {
    *type = FIELD;
    return true;
  }
  return false;
}

}  // namespace gdt
