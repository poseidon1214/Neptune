// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Eric Liu <ericliu@tencent.com>
// Created: 07/19/11
// Description: protobuf message json serializer

#include "common/encoding/cgi_to_pb.h"
#include <sstream>
#include <vector>
#include "app/qzap/common/base/string_utility.h"
#include "common/base/string/string_number.h"
#include "common/encoding/percent.h"
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#define SET_ERROR_INFO(error_var, error_val)    \
  do { if (error_var) *error_var = error_val; } while (0)

using namespace cgicc;

namespace gdt {

bool CgiToProtoMessage(
    const Cgicc& cgi,
    google::protobuf::Message* message,
    std::string* error,
    bool urlencoded) {

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
    const_form_iterator iter = cgi.getElement(field->name());
    if (iter == cgi.getElements().end()) {
      continue;
    }
    std::string value = **iter;

    if (field->is_repeated()) {
      SET_ERROR_INFO(error, "invalid type for array field " +
                     field->full_name() + ".");
      return false;
    }

    switch (field->cpp_type()) {
#define CASE_NUMERIC_FIELD(cpptype, method, valuetype) \
    case FieldDescriptor::CPPTYPE_##cpptype: { \
      valuetype number_value; \
      if (StringToNumeric(value, &number_value)) { \
        reflection->Set##method(message, field, number_value); \
      } else { \
        SET_ERROR_INFO(error, \
                       "invalid type for field " + \
                       field->full_name() + "."); \
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

}  // namespace gdt
