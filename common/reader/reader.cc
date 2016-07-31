// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include "common/reader/reader.h"
#include "app/qzap/common/utility/time_utility.h"
#include "app/qzap/common/base/string_utility.h"
#include "common/base/string/string_number.h"
#include "thirdparty/protobuf/descriptor.h"
#include "thirdparty/protobuf/message.h"

namespace gdt {
namespace Reader {

xmlNodePtr XmlReaderIterator::GetBeginNode(xmlNodePtr xml_node,
                                           const ParseConfig& config,
                                           uint32_t index = 0) {
  if (xml_node == NULL) {
    return NULL;
  }
  if (index == config.xml_node_name_size()) {
    return xml_node;
  }
  std::string node_name = config.xml_node_name(index);
  xmlNodePtr current_node = xml_node->xmlChildrenNode;
  while (current_node != NULL) {
    if (!xmlStrcasecmp(current_node->name, (const xmlChar *)(node_name.c_str()))) {
      return GetBeginNode(current_node, config, index + 1);
    }
    current_node = current_node->next;
  }  
  return NULL;
}

}  // namespace Reader
}  // namespace gdt
