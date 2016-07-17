// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)


#ifndef COMMON_PROTO_CONVERTER_H_
#define COMMON_PROTO_CONVERTER_H_

#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <istream>
#include "thirdparty/glog/logging.h"
#include "thirdparty/libxml/HTMLparser.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "common/file/file_tools.h"
#include "common/proto/config.pb.h"


namespace gdt {

typedef MappingConfig_FieldConfig FieldConfig;

typedef std::map<std::string, std::string (*)(const std::string)> FuncMap;

static FuncMap func_map;

bool GetRawFieldValue(const std::vector<std::string>& field,
                      const FieldConfig& field_config,
                      std::string* value);

bool GetRawFieldValue(xmlNodePtr xml_node,
                      const FieldConfig& field_config,
                      std::string* value,
                      uint32_t index = 0);

bool GetRawFieldValue(const google::protobuf::Message& message,
                      const FieldConfig& field_config,
                      std::string* value);

template <class T>
bool GetFieldValue(const T& instance,
                   const FieldConfig& field_config,
                   std::string* value) {
  if (!GetRawFieldValue(instance, field_config, value)) {
    return false;
  }

  if (field_config.has_callback()) {
    auto iter = func_map.find(field_config.callback());
    if (iter != func_map.end()) {
      *value = (*(iter->second))(*value);
    }
  }
  return true;
}


template <class T>
bool ConvertToProto(
    const T& instance,
    const MappingConfig& config,
    google::protobuf::Message* message) {
  std::map<std::string, FieldConfig> field_name_to_config;
  for (google::protobuf::RepeatedPtrField<FieldConfig>
      ::const_iterator iter = config.field_config().begin();
      iter != config.field_config().end(); iter++) {
    field_name_to_config[iter->field_name()] = *iter;
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
    std::string value;
    const FieldDescriptor* field = fields[i];
    auto iter = field_name_to_config.find(field->name());
    if (iter == field_name_to_config.end()) {
      continue;
    } else if (!GetFieldValue(instance, iter->second, &value)){
      continue;
    }
    switch (field->cpp_type()) {
#define CASE_NUMERIC_FIELD(cpptype, method, jsontype, valuetype) \
    case FieldDescriptor::CPPTYPE_##cpptype: { \
      if (field->is_repeated()) { \
        continue; \
      } else { \
        valuetype number_value; \
        if (value.empty()) { \
        } else if (StringToNumeric(value, &number_value)) { \
          reflection->Set##method(message, field, number_value); \
        } else { \
          continue; \
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
        continue;
      } else {
        reflection->SetBool(message, field, value != "0");
      }
      break;
    }

    case FieldDescriptor::CPPTYPE_STRING: {
      if (field->is_repeated()) {
        continue;
      } else {
        reflection->SetString(message, field, value);
      }
      break;
    }

    case FieldDescriptor::CPPTYPE_ENUM: {
      if (field->is_repeated()) {
        continue;
      } else {
        int number_value;
        if (!StringToNumeric(value, &number_value)) {
          continue;
        }
        const EnumValueDescriptor * enum_value_descriptor =
          field->enum_type()->FindValueByNumber(number_value);
        if (!enum_value_descriptor) {
          return false;
        }
        reflection->SetEnum(message, field, enum_value_descriptor);
      }
      break;
    }

    case FieldDescriptor::CPPTYPE_MESSAGE: {
      if (!ConvertToProto(instance, iter->second.mapping_config(),
          reflection->MutableMessage(message, field))) {
        return false;
      }
    }
    }
  }
  return true;
}


}  // namespace gdt

#define REGISTER_FUNC(function_name) \
  gdt::func_map[#function_name] = &function_name;

#endif  // COMMON_PROTO_CONVERTER_H_
