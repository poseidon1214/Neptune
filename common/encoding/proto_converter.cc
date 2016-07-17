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

}  // namespace gdt
