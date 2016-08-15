// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include "common/encoding/proto_converter.h"
#include "app/qzap/common/utility/time_utility.h"
#include "app/qzap/common/base/string_utility.h"
#include "common/base/string/string_number.h"
#include "thirdparty/protobuf/descriptor.h"
#include "thirdparty/protobuf/message.h"

namespace gdt {

bool GetRawFieldValue(const std::vector<std::string>& field,
                      const FieldConfig& field_config,
                      std::string* value) {
  uint32_t index = field_config.line_config().index();
  if (index < field.size()) {
  	*value = field[index];
    return true;
  } else {
  	return false;
  }
}

bool GetRawFieldValue(xmlNodePtr xml_node,
                      const FieldConfig& field_config,
                      std::string* value,
                      uint32_t index) {
  if (xml_node == NULL) {
    return false;
  }

  if (index == field_config.xml_config().node_name_size()) {
  	*value = reinterpret_cast<char*>(xmlNodeGetContent(xml_node));
  	return true;
  }
  std::string node_name = field_config.xml_config().node_name(index);
  xmlNodePtr current_node = xml_node->xmlChildrenNode;
  while (current_node != NULL) {
    if (!xmlStrcasecmp(current_node->name, (const xmlChar *)(node_name.c_str()))) {
      return GetRawFieldValue(current_node, field_config, value, index + 1);
    }
    current_node = current_node->next;
  } 
  return false;
}

bool GetRawFieldValue(const google::protobuf::Message&,
                      const FieldConfig& field_config,
                      std::string* value) {
  return true;
}

bool GetFieldValue(const google::protobuf::Message* message,
    const google::protobuf::RepeatedPtrField<std::string> &source_fields,
    FieldValue *filed_value) {
  if (message == NULL || filed_value == NULL) {
    return false;
  }
  using namespace std;  // NOLINT(build/namespaces)
  using namespace google::protobuf;  // NOLINT(build/namespaces)
  for (int i = 0; i < source_fields.size() - 1; i++) {
    const string& field_name = source_fields.Get(i);
    const Reflection *reflection = message->GetReflection();
    const Descriptor *descriptor = message->GetDescriptor();
    const FieldDescriptor *field = descriptor->FindFieldByName(field_name);
    if (field->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE) {
      return false;
    }
    message = &(reflection->GetMessage(*message, field));
  }

  const Reflection *reflection = message->GetReflection();
  const Descriptor *descriptor = message->GetDescriptor();
  const FieldDescriptor *field = descriptor->FindFieldByName(
    source_fields.Get(source_fields.size() - 1));
  if (field == NULL) {
    return false;
  }
  if (field->is_repeated()) {
    LOG(ERROR) << "feature is repeated , feature name = "
      << source_fields.Get(source_fields.size() - 1);
    return false;
  }
  if (field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
    return false;
  }
  if (field->cpp_type() == FieldDescriptor::CPPTYPE_INT32) {
    filed_value->set_data_type(FieldValue::INT32);
    filed_value->set_int32_value(reflection->GetInt32(*message, field));
  } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_INT64) {
    filed_value->set_data_type(FieldValue::INT64);
    filed_value->set_int64_value(reflection->GetInt64(*message, field));
  } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_UINT32) {
    filed_value->set_data_type(FieldValue::UINT32);
    filed_value->set_uint32_value(reflection->GetUInt32(*message, field));
  } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_UINT64) {
    filed_value->set_data_type(FieldValue::UINT64);
    filed_value->set_uint64_value(reflection->GetUInt64(*message, field));
  } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_DOUBLE) {
    filed_value->set_data_type(FieldValue::DOUBLE);
    filed_value->set_double_value(reflection->GetDouble(*message, field));
  } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_FLOAT) {
    filed_value->set_data_type(FieldValue::FLOAT);
    filed_value->set_float_value(reflection->GetFloat(*message, field));
  } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_BOOL) {
    filed_value->set_data_type(FieldValue::BOOL);
    filed_value->set_bool_value(reflection->GetBool(*message, field));
  } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_ENUM) {
    // 使用枚举的值进行处理
    filed_value->set_data_type(FieldValue::UINT32);
    filed_value->set_uint32_value(
      reflection->GetEnum(*message, field)->number());
  } else if (field->cpp_type() == FieldDescriptor::CPPTYPE_STRING) {
    filed_value->set_data_type(FieldValue::STRING);
    filed_value->set_string_value(reflection->GetString(*message, field));
  } else {
    return false;
  }
  return true;
}

}  // namespace gdt
