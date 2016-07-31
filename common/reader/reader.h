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
#include "data_storer/kv/leveldb/proto_kv.h"
#include "data_storer/sql/mysql_handler.h"

namespace gdt {
namespace Reader {

class ReaderIterator {
 public:
  virtual ReaderIterator* Begin() = 0;
  virtual ReaderIterator* Next() = 0;
  virtual bool Get(google::protobuf::Message* message) = 0;
};

class LineReaderIterator : public ReaderIterator {
 public:
  LineReaderIterator(const std::string filename, const MappingConfig* config) {
    filename_ = filename;
    config_ = config;
  }
  virtual ReaderIterator* Begin() {
    ifs_.open(filename_.c_str());
    return Next();
  }

  virtual ReaderIterator* Next() {
    return getline(ifs_, line_) ? static_cast<ReaderIterator*>(this) : NULL;
  }
  virtual bool Get(google::protobuf::Message* message) {
    std::vector<std::string> values;
    SplitString(line_, "\t", &values);
    return ConvertToProto(values, *config_, message);
  }
 public:
  // 当前行
  std::string line_;
  // 当前行
  std::ifstream ifs_;
  // 配置
  const MappingConfig* config_;
  // 文件名
  std::string filename_;
};

class XmlReaderIterator : public ReaderIterator {
 public:
  XmlReaderIterator(const std::string filename, const ParseConfig* config) {
    filename_ = filename;
    config_ = config;
  }
  virtual ~XmlReaderIterator() {
    if (doc_ != NULL) {
      xmlFreeDoc(doc_);
    }
  }

  virtual ReaderIterator* Begin() {
    doc_ = xmlReadFile(filename_.c_str(), config_->charset().c_str(), XML_PARSE_RECOVER);
    if (doc_ == NULL || config_->xml_node_name().empty()) {
      LOG(ERROR) << "(doc_ == NULL || config_->xml_node_name().empty())";
      return NULL;
    }
    current_node_ = xmlDocGetRootElement(doc_);
    current_node_ = GetBeginNode(current_node_, *config_);
    instance_node_name_ = config_->xml_node_name(config_->xml_node_name_size() - 1);
    return current_node_ != NULL ? static_cast<ReaderIterator*>(this) : NULL;
  }

  virtual ReaderIterator* Next() {
    if (!xmlStrcasecmp(current_node_->name, (const xmlChar *)(instance_node_name_.c_str()))) {
      current_node_ = current_node_->next;
      return current_node_ != NULL ? Next() : NULL;
    }
    current_node_ = current_node_->next;
    return current_node_ != NULL ? static_cast<ReaderIterator*>(this) : NULL;
  }

  virtual bool Get(google::protobuf::Message* message) {
    return ConvertToProto(current_node_, config_->mapping_config(), message);
  }

 private:
  // 获取开始节点
  xmlNodePtr GetBeginNode(xmlNodePtr xml_node,
                          const ParseConfig& config,
                          uint32_t index = 0);
 public:
  // 配置
  const ParseConfig* config_;
  // 文件名
  std::string filename_;
  // 文件指针
  xmlDocPtr doc_;
  // 当前节点
  xmlNodePtr current_node_;
  // 节点名
  std::string instance_node_name_;
};

class Reader {
 public:
  ~Reader() {
    if (iterator != NULL) {
      delete iterator;
    }
  }

  bool Init(const IOConfig& config) {
    std::string filename = GetFormatFilename(config.file_pattern(),
                                             config.seconds_ago());
    config_.CopyFrom(config); 
    LOG(INFO) << "Read From File:" << filename;
    switch (config.store_method()) {
      case LevelDb:
        break;
      case Mysql:
        return true;
      case SSTable:
        break;
      case File:
        return InitFileConfig(filename, config.parse_config());
      default:
        return false;
    }
    return true;
  }


  bool InitFileConfig(const std::string& filename,
                      const ParseConfig& config) {
    CHECK(FileExisting(filename));
    switch (config.parse_method()) {
      case Xml:
        iterator = new XmlReaderIterator(filename, &config);
        break;
      case Line:
        iterator = new LineReaderIterator(filename, &config.mapping_config());
        break;
      default:
        return false;
    }
    return iterator != NULL;
  }

  ReaderIterator* Begin() {
    return iterator->Begin();
  }
 
  ReaderIterator* End() {
    return NULL;
  }

  template <class T>
  bool Load(std::vector<T>* data) {
    switch (config_.store_method()) {
      case LevelDb:
        break;
      case Mysql:
        return true;
      case SSTable:
        break;
      case File:
        return LoadFromIterator(data);
      default:
        return false;
    }
  }

  template <class T>
  bool LoadFromIterator(std::vector<T>* data) {
    for (ReaderIterator* iter = this->Begin(); 
           iter != this->End(); iter = iter->Next()) {
      T message;
      if (iter->Get(&message)) {
        data->push_back(message);
      }
    }
    return true;
  }

  template <class T>
  bool LoadFromMysql(std::vector<T>* data) {
    MysqlHandler mysql_handler;
    mysql_handler.Open(config_.mysql_config());
    Lambda lambda;
    return mysql_handler.Select(lambda, data);;
  }

 public:
  // 迭代器
  ReaderIterator* iterator;
  // 配置
  IOConfig config_;
};

  // 从文件里读取
template <class T>
bool ReadFromIO(const IOConfig& config,
                std::vector<T>* data) {
  Reader reader;
  CHECK(reader.Init(config));
  CHECK(reader.Load(data));
  return true;
}

template <class T>
bool ReadFromFile(const std::string& filename,
                  const ParseConfig& config,
                  std::vector<T>* data) {
  Reader reader;
  CHECK(reader.InitFileConfig(filename, config));
  CHECK(reader.LoadFromIterator(data));
  return true;
}

/*

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
  bool ReadFromMysql(const IOConfig& config,
                     std::vector<T>* data) {
    MysqlHandler mysql_handler;
    mysql_handler.Open(config.mysql_config());
    Lambda lambda;
    return mysql_handler.Select(lambda, data);;
  }
  // 从文件里读取
  template <class T>
  bool ReadFromIO(const IOConfig& config,
                  std::vector<T>* data) {
    std::string filename = GetFormatFilename(config.file_pattern(),
                                             config.seconds_ago());
    LOG(INFO) << "Read From File:" << filename;
    LOG(ERROR) << "data:" << data;
    switch (config.store_method()) {
      case LevelDb:
        break;
      case Mysql:
        return ReadFromMysql(config, data);
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
*/
}  // namespace Reader
}  // namespace gdt

#endif  // COMMON_READER_READER_H_
