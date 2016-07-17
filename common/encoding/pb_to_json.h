// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Eric Liu <ericliu@tencent.com>
// Created: 07/19/11
// Description: protobuf message json serializer

#ifndef COMMON_ENCODING_PB_TO_JSON_H_
#define COMMON_ENCODING_PB_TO_JSON_H_

#include <string>

namespace Json {
class Value;
}  // namespace Json

namespace google {
namespace protobuf {
class Message;
}  // namespace protobuf
}  // namespace google

namespace gdt {

bool ProtoMessageToJsonValue(
    const google::protobuf::Message& message,
    Json::Value* json_value,
    std::string* error = NULL);

/**
 * @brief Convert protobuf message object to json string.
 * @param message protobuf message object.
 * @param json_string [out] Contains the result string.
 *                    if successfully converted.
 * @param enable_styled  true result string include format
 *                            characters LF and Space, friendly to read.
 *                       false discard format characters.
 * @retval true  if successfully converted
 * @retval false if an error occurred.
 */
bool ProtoMessageToJson(
    const google::protobuf::Message& message,
    std::string* json_string,
    std::string* error = NULL,
    bool enable_styled = false);

}  // namespace gdt

#endif  // COMMON_ENCODING_PB_TO_JSON_H_
