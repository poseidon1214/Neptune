// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/encoding/proto_json_format.h"

#include <string>
#include "common/encoding/pb_to_json.h"
#include "common/encoding/json_to_pb.h"
#include "thirdparty/jsoncpp/reader.h"
#include "thirdparty/jsoncpp/value.h"
#include "thirdparty/jsoncpp/writer.h"

namespace gdt {

bool ProtoJsonFormat::WriteToValue(const google::protobuf::Message& message,
                                   Json::Value* output, std::string* error) {
  return ProtoMessageToJsonValue(message, output, error);
}

bool ProtoJsonFormat::PrintToStyledString(
    const google::protobuf::Message& message,
    std::string* output, std::string* error) {
  return ProtoMessageToJson(message, output, error, true);
}

bool ProtoJsonFormat::PrintToFastString(
    const google::protobuf::Message& message,
    std::string* output, std::string* error) {
  return ProtoMessageToJson(message, output, error, false);
}

bool ProtoJsonFormat::PrintToString(const google::protobuf::Message& message,
                                    std::string* output, std::string* error) {
  return PrintToFastString(message, output, error);
}

bool ProtoJsonFormat::ParseFromString(const std::string& input,
                                      google::protobuf::Message* output,
                                      std::string* error) {
  return JsonToProtoMessage(input, output, error);
}

bool ProtoJsonFormat::ReadFromValue(const Json::Value& input,
                                    google::protobuf::Message* output,
                                    std::string* error) {
  return JsonValueToProtoMessage(input, output, error);
}

}  // namespace gdt
