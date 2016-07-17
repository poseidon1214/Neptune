// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Eric Liu <ericliu@tencent.com>
// Created: 07/19/11
// Description: protobuf message json deserializer

#ifndef COMMON_PB_TO_MAP_H
#define COMMON_PB_TO_MAP_H

#include <string>
#include <map>

#include "common/base/string/string_piece.h"

namespace Json {
class Value;
}  // namespace Json

namespace google {
namespace protobuf {
class Message;
}  // namespace protobuf
}  // namespace google

namespace gdt {

bool MapToProtoMessage(
    const std::map<std::string, std::string>& parameters,
    google::protobuf::Message* message,
    std::string* error = NULL,
    bool urlencoded = false);


}  // namespace gdt

#endif  // COMMON_PB_TO_MAP_H
