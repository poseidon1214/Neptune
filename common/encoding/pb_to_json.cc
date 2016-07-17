// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Eric Liu <ericliu@tencent.com>
// Created: 07/19/11
// Description: protobuf message json serializer

#include "common/encoding/pb_to_json.h"

#include <string>
#include <vector>
#include "app/qzap/common/base/string_utility.h"
#include "common/base/string/string_number.h"
#include "common/encoding/percent.h"
#include "thirdparty/jsoncpp/reader.h"
#include "thirdparty/jsoncpp/value.h"
#include "thirdparty/jsoncpp/writer.h"
#include "thirdparty/protobuf/descriptor.h"
#include "thirdparty/protobuf/message.h"

namespace gdt {

bool ProtoMessageToJsonValue(
    const google::protobuf::Message& message,
    Json::Value* json_value,
    std::string* error) {
  using namespace std;  // NOLINT(build/namespaces)
  using namespace google::protobuf;  // NOLINT(build/namespaces)

  if (!message.IsInitialized()) {
    *error = message.InitializationErrorString();
    return false;
  }

  const Reflection* reflection = message.GetReflection();

  vector<const FieldDescriptor*> fields;
  reflection->ListFields(message, &fields);

  for (size_t i = 0; i < fields.size(); i++) {
    const FieldDescriptor* field = fields[i];
    switch (field->cpp_type()) {
#define CASE_FIELD_TYPE(cpptype, method, jsontype) \
      case FieldDescriptor::CPPTYPE_##cpptype: { \
        if (field->is_repeated()) { \
          int field_size = reflection->FieldSize(message, field); \
          for (int index = 0; index < field_size; index++) { \
            (*json_value)[field->name()].append( \
                              static_cast<jsontype>( \
                                  reflection->GetRepeated##method( \
                                      message, field, index))); \
          } \
        }  else { \
          (*json_value)[field->name()] = static_cast<jsontype>( \
                          reflection->Get##method( \
                              message, field)); \
        } \
        break; \
      }

      CASE_FIELD_TYPE(INT32,  Int32,  Json::Value::Int);
      CASE_FIELD_TYPE(UINT32, UInt32, Json::Value::UInt);
      CASE_FIELD_TYPE(FLOAT,  Float,  float);
      CASE_FIELD_TYPE(DOUBLE, Double, double);
      CASE_FIELD_TYPE(BOOL,   Bool,   bool);
#undef CASE_FIELD_TYPE

#define CASE_64BIT_INT_FIELD(cpptype, method, valuetype) \
      case FieldDescriptor::CPPTYPE_##cpptype: { \
        if (field->is_repeated()) { \
          int field_size = reflection->FieldSize(message, field); \
          for (int index = 0; index < field_size; index++) { \
            valuetype number_value = \
                reflection->GetRepeated##method(message, field, index); \
            (*json_value)[field->name()].append( \
                ConvertToString(number_value)); \
          } \
        }  else { \
          valuetype number_value = reflection->Get##method(message, field); \
          (*json_value)[field->name()] = ConvertToString(number_value); \
        } \
        break; \
      } \

      CASE_64BIT_INT_FIELD(INT64,  Int64,  int64_t);
      CASE_64BIT_INT_FIELD(UINT64, UInt64, uint64_t);
#undef CASE_64BIT_INT_FIELD

      case FieldDescriptor::CPPTYPE_STRING: {
        string field_value;
        if (field->is_repeated()) {
          int field_size = reflection->FieldSize(message, field);
          for (int index = 0; index < field_size; index++) {
            const string& value = reflection->GetRepeatedStringReference(
                message, field, index, &field_value);
            const string* value_ptr = &value;
            string encode_value;
            if (field->type() == FieldDescriptor::TYPE_BYTES) {
              encode_value = PercentEncoding::EncodeUriComponent(value);
              value_ptr = &encode_value;
            }
            (*json_value)[field->name()].append(*value_ptr);
          }
        } else {
          const string& value = reflection->GetStringReference(
              message, field, &field_value);
          const string* value_ptr = &value;
          string encode_value;
          if (field->type() == FieldDescriptor::TYPE_BYTES) {
            encode_value = PercentEncoding::EncodeUriComponent(value);
            value_ptr = &encode_value;
          }
          (*json_value)[field->name()] = *value_ptr;
        }
        break;
      }
      case FieldDescriptor::CPPTYPE_ENUM: {
        if (field->is_repeated()) {
          int field_size = reflection->FieldSize(message, field);
          for (int index = 0; index < field_size; index++) {
            (*json_value)[field->name()].append(
                reflection->GetRepeatedEnum(message, field, index)->number());
          }
        } else {
          (*json_value)[field->name()] = reflection->GetEnum(
              message, field)->number();
        }
        break;
      }
      case FieldDescriptor::CPPTYPE_MESSAGE: {
        if (field->is_repeated()) {
          int field_size = reflection->FieldSize(message, field);
          for (int index = 0; index < field_size; index++) {
            Json::Value value;
            if (!ProtoMessageToJsonValue(
                reflection->GetRepeatedMessage(message, field, index),
                &value, error)) {
              return false;
            }
            (*json_value)[field->name()].append(value);
          }
        } else {
          Json::Value value;
          if (!ProtoMessageToJsonValue(
              reflection->GetMessage(message, field),
              &value, error)) {
            return false;
          }
          (*json_value)[field->name()] = value;
        }
        break;
      }
    }
  }
  return true;
}

bool ProtoMessageToJson(
    const google::protobuf::Message& message,
    std::string* json_string,
    std::string* error,
    bool enable_styled) {
  Json::Value root;
  if (ProtoMessageToJsonValue(message, &root, error)) {
    if (enable_styled) {
      Json::StyledWriter styled_writer;
      *json_string = styled_writer.write(root);
    } else {
      Json::FastWriter fast_writer;
      *json_string = fast_writer.write(root);
    }
    return true;
  }
  return false;
}

}  // namespace gdt
