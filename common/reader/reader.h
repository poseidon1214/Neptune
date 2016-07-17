// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)


#ifndef COMMON_READER_READER_H_
#define COMMON_READER_READER_H_

#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <istream>
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "common/encoding/proto_converter.h"
#include "common/file/file_tools.h"
#include "common/proto/config.pb.h"

namespace gdt {
namespace Reader {
  // 获取开始节点
  xmlNodePtr GetBeginNode(xmlNodePtr xml_node,
                          const ParseConfig& config,
                          uint32_t index = 0);
  //
  template <class T>
  bool ReadFromLine(const std::string& filename,
                    const MappingConfig& config,
                    std::vector<T>* data) {
    std::ifstream ifs(filename.c_str());
    std::string line;
    uint64_t error_num = 0;
    LOG(INFO) << "Reading " << filename ;
    while (getline(ifs, line)) {
      T message;
      std::vector<std::string> values;
      SplitString(line, "\t", &values);
      if (ConvertToProto(values, config, &message)) {
        data->push_back(message);
      } else {
        error_num++;
      }
    }
    if (error_num > 0) {
      LOG(ERROR) << "Read " << filename << "  Line Fromat Error:" << error_num;
      return false;
    }
    return true;
  }
  template <class T>
  bool ReadFromXml(const std::string& filename,
                   const ParseConfig& config,
                   std::vector<T>* data) {
    xmlDocPtr doc;
    xmlNodePtr current_node;
    doc = xmlReadFile(filename.c_str(), config.charset().c_str(), XML_PARSE_RECOVER);
    if (doc == NULL) {
      LOG(ERROR) << "Xml not parsed successfully:" << filename;
      return false;
    }
    current_node = xmlDocGetRootElement(doc);
    current_node = GetBeginNode(current_node, config);
    if (current_node == NULL) {
      LOG(ERROR) << "Get Begin Node False:" << filename;
      xmlFreeDoc(doc);
      return false;
    }
    if (config.xml_node_name().empty()) {
      return false;
    }
    std::string instance_node_name = config.xml_node_name(config.xml_node_name_size() - 1);
    while (current_node != NULL) {
      if (xmlStrcasecmp(current_node->name, (const xmlChar *)(instance_node_name.c_str()))) {
        current_node = current_node->next;
        continue;
      }
      T message;
      if (ConvertToProto(current_node, config.mapping_config(), &message)) {
        data->push_back(message);
      }
      current_node = current_node->next;
    } 
    return true;
  }
  // 从文件里读取
  template <class T>
  bool ReadFromFile(const std::string& filename,
                    const ParseConfig& config,
                    std::vector<T>* data) {
    if (!FileExisting(filename)) {
      LOG(ERROR) << "File not exist:" << filename;
      return false;
    }
    switch (config.parse_method()) {
      case Xml:
        return ReadFromXml(filename, config, data);
      case Line:
        return ReadFromLine(filename, config.mapping_config(), data);
      default:
        return false;
    }
    return true;
  }
  // 从文件里读取
  template <class T>
  bool ReadFromIO(const IOConfig& config,
                  std::vector<T>* data) {
    std::string filename = GetFormatFilename(config.file_pattern(),
                                             config.seconds_ago());
    LOG(INFO) << "Read From File:" << filename;
    switch (config.store_method()) {
      case LevelDb:
        break;
      case SSTable:
        break;
      case File:
        return ReadFromFile(filename, config.parse_config(), data);
      default:
        return false;
    }
    return true;
  }
};

}  // namespace gdt

#endif  // COMMON_READER_READER_H_
