// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Eric Liu <ericliu@tencent.com>
// Created: 07/19/11
// Description: protobuf message json serializer

#include "common/encoding/json_to_pb.h"
#include <sstream>
#include <vector>
#include "app/qzap/common/base/string_utility.h"
#include "common/base/string/string_number.h"
#include "common/encoding/percent.h"
#include "thirdparty/jsoncpp/reader.h"
#include "thirdparty/jsoncpp/value.h"
#include "thirdparty/jsoncpp/writer.h"
#include "thirdparty/protobuf/descriptor.h"
#include "thirdparty/protobuf/message.h"

#define SET_ERROR_INFO(error_var, error_val)    \
  do { if (error_var) *error_var = error_val; } while (0)

namespace gdt {

bool JsonValueToProtoMessage(
    const Json::Value& json_value,
    google::protobuf::Message* message,
    std::string* error,
    bool urlencoded) {
  if (json_value.type() != Json::objectValue) {
    SET_ERROR_INFO(error, "type of json_value is not object.");
    return false;
  }

  using namespace std;  // NOLINT(build/namespaces)
  using namespace google::protobuf;  // NOLINT(build/namespaces)

  const Reflection* reflection = message->GetReflection();
  const Descriptor* descriptor = message->GetDescriptor();

  vector<const FieldDescriptor*> fields;
  for (int i = 0; i < descriptor->extension_range_count(); i++) {
    const Descriptor::ExtensionRange* ext_range =
        descriptor->extension_range(i);
    for (int tag_number = ext_range->start; tag_number < ext_range->end;
         tag_number++) {
      const FieldDescriptor* field = reflection->FindKnownExtensionByNumber(
          tag_number);
      if (!field) continue;
      fields.push_back(field);
    }
  }
  for (int i = 0; i < descriptor->field_count(); i++) {
    fields.push_back(descriptor->field(i));
  }

  for (size_t i = 0; i < fields.size(); i++) {
    const FieldDescriptor* field = fields[i];
    Json::Value value = json_value[field->name()];

    if (value.isNull()) {
      if (field->is_required()) {
        SET_ERROR_INFO(error, "missed required field " +
                       field->full_name() + ".");
        return false;
      }
      continue;
    }
    if (field->is_repeated()) {
      if (!value.isArray()) {
        SET_ERROR_INFO(error, "invalid type for array field " +
                       field->full_name() + ".");
        return false;
      }
    }

#define VALUE_TYPE_CHECK(value, jsontype) \
    if (!value.is##jsontype()) { \
      SET_ERROR_INFO(error, \
                     "invalid type for field " + \
                     field->full_name() + "."); \
      return false; \
    }

    switch (field->cpp_type()) {
#define CASE_NUMERIC_FIELD(cpptype, method, jsontype, valuetype) \
    case FieldDescriptor::CPPTYPE_##cpptype: { \
      if (field->is_repeated()) { \
        for (int index = 0; \
             index < static_cast<int>(value.size()); \
             index++) { \
          Json::Value item = value[Json::ArrayIndex(index)]; \
          if (item.is##jsontype()) { \
            reflection->Add##method(message, field, item.as##jsontype()); \
          } else if (item.isString()) { \
            valuetype number_value; \
            if (item.asString().empty()) { \
            } else if (StringToNumeric(item.asString(), &number_value)) { \
              reflection->Add##method(message, field, number_value); \
            } else { \
              SET_ERROR_INFO(error, \
                             "invalid type for field " + \
                             field->full_name() + "."); \
              return false; \
            } \
          } else { \
            SET_ERROR_INFO(error, "invalid type field " + \
                           field->full_name() + "."); \
            return false; \
          } \
        } \
      } else { \
        if (value.is##jsontype()) { \
          reflection->Set##method(message, field, value.as##jsontype()); \
        } else if (value.isString()) { \
          valuetype number_value; \
          if (value.asString().empty()) { \
          } else if (StringToNumeric(value.asString(), &number_value)) { \
            reflection->Set##method(message, field, number_value); \
          } else { \
            SET_ERROR_INFO(error, \
                           "invalid type for field " + \
                           field->full_name() + "."); \
            return false; \
          } \
        } else { \
          SET_ERROR_INFO(error, "invalid type for field " + \
                         field->full_name() + "."); \
          return false; \
        } \
      } \
      break; \
    }

    CASE_NUMERIC_FIELD(INT32,  Int32,  Int, int32_t);
    CASE_NUMERIC_FIELD(UINT32, UInt32, UInt, uint32_t);
    CASE_NUMERIC_FIELD(FLOAT,  Float,  Double, float);
    CASE_NUMERIC_FIELD(DOUBLE, Double, Double, double);
    CASE_NUMERIC_FIELD(INT64,  Int64,  Int64,  int64_t);
    CASE_NUMERIC_FIELD(UINT64, UInt64, UInt64, uint64_t);
#undef CASE_NUMERIC_FIELD

    case FieldDescriptor::CPPTYPE_BOOL: {
      if (field->is_repeated()) {
        for (int index = 0;
             index < static_cast<int>(value.size());
             index++) {
          Json::Value item = value[Json::ArrayIndex(index)];
          VALUE_TYPE_CHECK(item, Bool);
          reflection->AddBool(message, field,
                              item.asBool());
        }
      } else {
        VALUE_TYPE_CHECK(value, Bool);
        reflection->SetBool(message, field,
                            value.asBool());
      }
      break;
    }

    case FieldDescriptor::CPPTYPE_STRING: {
      if (field->is_repeated()) {
        for (int index = 0;
             index < static_cast<int>(value.size());
             index++) {
          Json::Value item = value[Json::ArrayIndex(index)];
          VALUE_TYPE_CHECK(item, String);
          string str = item.asString();
          if ((field->type() == FieldDescriptor::TYPE_BYTES || urlencoded) &&
              !PercentEncoding::Decode(&str)) {
            SET_ERROR_INFO(error,
                           "percent decode for string field " +
                           field->full_name() + ".");
            return false;
          }
          reflection->AddString(message, field, str);
        }
      } else {
        VALUE_TYPE_CHECK(value, String);
        string str = value.asString();
        if ((field->type() == FieldDescriptor::TYPE_BYTES || urlencoded) &&
            !PercentEncoding::Decode(&str)) {
          SET_ERROR_INFO(error,
                         "percent decode for string field " +
                         field->full_name() + ".");
          return false;
        }
        reflection->SetString(
          message, field, str);
      }
      break;
    }

    case FieldDescriptor::CPPTYPE_ENUM: {
      if (field->is_repeated()) {
        for (int index = 0;
             index < static_cast<int>(value.size());
             index++) {
          Json::Value item = value[Json::ArrayIndex(index)];
          const EnumValueDescriptor * enum_value_descriptor = NULL;
          if (item.isString()) {
            enum_value_descriptor =
              field->enum_type()->FindValueByName(item.asString());
            if (!enum_value_descriptor) {
              //  try to find value by converting name to int
              int number = 0;
              bool ret = StringToNumeric(item.asString(), &number);
              if (ret) {
                enum_value_descriptor =
                  field->enum_type()->FindValueByNumber(number);
              }
            }
          } else if (item.isInt()) {
            enum_value_descriptor =
              field->enum_type()->FindValueByNumber(item.asInt());
          } else {
            SET_ERROR_INFO(error,
                           "invalid type for field " +
                           field->full_name() + ".");
            return false;
          }

          if (!enum_value_descriptor) {
            SET_ERROR_INFO(error,
                           "invalid value for enum field " +
                           field->full_name() + ".");
            return false;
          }
          reflection->AddEnum(
            message, field,
            enum_value_descriptor);
        }
      } else {
        const EnumValueDescriptor * enum_value_descriptor = NULL;
        if (value.isString()) {
          enum_value_descriptor =
            field->enum_type()->FindValueByName(value.asString());
          if (!enum_value_descriptor) {
            //  try to find value by converting name to int
            int number = 0;
            bool ret = StringToNumeric(value.asString(), &number);
            if (ret) {
              enum_value_descriptor =
                field->enum_type()->FindValueByNumber(number);
            }
          }
        } else if (value.isInt()) {
          enum_value_descriptor =
            field->enum_type()->FindValueByNumber(value.asInt());
        } else {
          SET_ERROR_INFO(error,
                         "invalid type for field " + field->full_name() + ".");
          return false;
        }

        if (!enum_value_descriptor) {
          SET_ERROR_INFO(error,
                         "invalid value for enum field " +
                         field->full_name() + ".");
          return false;
        }
        reflection->SetEnum(
          message, field,
          enum_value_descriptor);
      }
      break;
    }

    case FieldDescriptor::CPPTYPE_MESSAGE: {
      if (field->is_repeated()) {
        for (int index = 0;
             index < static_cast<int>(value.size());
             index++) {
          Json::Value item = value[Json::ArrayIndex(index)];
          if (item.isObject()) {
            if (!JsonValueToProtoMessage(item,
                                         reflection->AddMessage(message, field),
                                         error))
              return false;

          } else {
            SET_ERROR_INFO(error,
                           "invalid type for field " +
                           field->full_name() + ".");
            return false;
          }
        }
      } else {
        if (!JsonValueToProtoMessage(value,
                                     reflection->MutableMessage(message, field),
                                     error))
          return false;
      }
      break;
    }
    }
  }
  return true;
}

bool JsonToProtoMessage(
    const StringPiece& json_string_piece,
    google::protobuf::Message* message,
    std::string* error,
    bool urlencoded) {
  Json::Value root;
  Json::Reader reader;
  if (!reader.parse(json_string_piece.data(),
                    json_string_piece.data() + json_string_piece.size(),
                    root)) {
    SET_ERROR_INFO(error, "json string format error.");
    return false;
  }

  return JsonValueToProtoMessage(root, message, error, urlencoded);
}

}  // namespace gdt
