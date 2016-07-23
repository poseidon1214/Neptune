// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Qian Wang<cernwang@tencent.com>


#include "common/encoding/pb_to_map.h"
#include <sstream>
#include <vector>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include "app/qzap/common/base/string_utility.h"
#include "common/base/string/string_number.h"
#include "common/encoding/percent.h"

#define SET_ERROR_INFO(error_var, error_val)    \
  do { if (error_var) *error_var = error_val; } while (0)

using namespace std;  // NOLINT(build/namespaces)
using namespace google::protobuf;  // NOLINT(build/namespaces)


namespace gdt {

template <class T>
static std::string ConvertToString(const T& t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}

bool MapToProtoMessage(
    const std::map<std::string, std::string>& parameters,
    google::protobuf::Message* message) {

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
    std::map<std::string, std::string>::const_iterator iter = parameters.find(field->name());
    if (iter == parameters.end()) {
      return false;
    }
    std::string value = iter->second;

    if (field->is_repeated()) {
      return false;
    }

    switch (field->cpp_type()) {
#define CASE_NUMERIC_FIELD(cpptype, method, valuetype) \
    case FieldDescriptor::CPPTYPE_##cpptype: { \
      valuetype number_value; \
      if (StringToNumeric(value, &number_value)) { \
        reflection->Set##method(message, field, number_value); \
      } else { \
        return false; \
      } \
      break; \
    }

    CASE_NUMERIC_FIELD(INT32,  Int32,  int32_t);
    CASE_NUMERIC_FIELD(UINT32, UInt32, uint32_t);
    CASE_NUMERIC_FIELD(FLOAT,  Float,  float);
    CASE_NUMERIC_FIELD(DOUBLE, Double, double);
    CASE_NUMERIC_FIELD(INT64,  Int64,  int64_t);
    CASE_NUMERIC_FIELD(UINT64, UInt64, uint64_t);
#undef CASE_NUMERIC_FIELD

    case FieldDescriptor::CPPTYPE_BOOL: {
      if (value == "true") {
        reflection->SetBool(message, field, true);
      } else if (value == "false") {
        reflection->SetBool(message, field, false);
      }
      break;
    }

    case FieldDescriptor::CPPTYPE_STRING: {
      reflection->SetString(message, field, value);
      break;
    }

    case FieldDescriptor::CPPTYPE_ENUM: {
      int32_t enum_value;
      if (!StringToNumeric(value, &enum_value)) {
        return false;
      }
      const EnumValueDescriptor * enum_value_descriptor =
          field->enum_type()->FindValueByNumber(enum_value);
        reflection->SetEnum(
          message, field,
          enum_value_descriptor);
      break;
    };
    }
  }
  return true;
}

bool ProtoMessageToMap(
  const google::protobuf::Message& message,
  std::map<std::string, std::string>* parameters) {

  const Reflection* reflection = message.GetReflection();
  vector<const FieldDescriptor*> fields;
  reflection->ListFields(message, &fields);

  for (size_t i = 0; i < fields.size(); i++) {
  const FieldDescriptor* field = fields[i];
  switch (field->cpp_type()) {
#define CASE_FIELD_TYPE(cpptype, method) \
    case FieldDescriptor::CPPTYPE_##cpptype: { \
    if (field->is_repeated()) { \
      int field_size = reflection->FieldSize(message, field); \
      for (int index = 0; index < field_size; index++) { \
      (*parameters)[field->name()] += \
                ConvertToString( \
                  reflection->GetRepeated##method( \
                    message, field, index)) + ";"; \
      } \
    }  else { \
      (*parameters)[field->name()] = ConvertToString( \
              reflection->Get##method( \
                message, field)); \
    } \
    break; \
    }

    CASE_FIELD_TYPE(INT32,  Int32);
    CASE_FIELD_TYPE(UINT32, UInt32);
    CASE_FIELD_TYPE(DOUBLE, Double);
    CASE_FIELD_TYPE(INT64,  Int64);
    CASE_FIELD_TYPE(UINT64, UInt64);
    CASE_FIELD_TYPE(BOOL, Bool);
    CASE_FIELD_TYPE(FLOAT,  Float);

#undef CASE_FIELD_TYPE

    case FieldDescriptor::CPPTYPE_STRING: {
    string field_value;
    if (field->is_repeated()) {
      int field_size = reflection->FieldSize(message, field);
      for (int index = 0; index < field_size; index++) {
      const string& value = reflection->GetRepeatedStringReference(
        message, field, index, &field_value);
      const string* value_ptr = &value;
      (*parameters)[field->name()] += (*value_ptr);
      }
    } else {
      const string& value = reflection->GetStringReference(
        message, field, &field_value);
      const string* value_ptr = &value;
      (*parameters)[field->name()] = *value_ptr;
    }
    break;
    }
    case FieldDescriptor::CPPTYPE_ENUM: {
    if (field->is_repeated()) {
      int field_size = reflection->FieldSize(message, field);
      for (int index = 0; index < field_size; index++) {
      (*parameters)[field->name()] += ConvertToString(
        reflection->GetRepeatedEnum(message, field, index)->number()) + ";";
      }
    } else {
      (*parameters)[field->name()] = reflection->GetEnum(
        message, field)->number();
    }
    break;
    }
    case FieldDescriptor::CPPTYPE_MESSAGE: {
    if (field->is_repeated()) {
      int field_size = reflection->FieldSize(message, field);
      for (int index = 0; index < field_size; index++) {
      std::string text;
      if (!google::protobuf::TextFormat::PrintToString(
        reflection->GetRepeatedMessage(message, field, index), &text)) {
        return false;
      }
      (*parameters)[field->name()] += text;
      }
    } else {
      std::string text;
      if (!google::protobuf::TextFormat::PrintToString(
        reflection->GetMessage(message, field), &text)) {
      return false;
      }
      (*parameters)[field->name()] = text;
    }
    break;
    }
  }
  }
  return true;
}

}  // namespace gdt
