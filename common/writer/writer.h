// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)


#ifndef COMMON_WRITER_WRITER_H_
#define COMMON_WRITER_WRITER_H_

#include <string>
#include <set>
#include <vector>
#include <ostream>
#include <algorithm>
#include <functional>
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "common/proto/config.pb.h"
#include "data_storer/sql/mysql_handler.h"

namespace gdt {
namespace Writer {

  // 重载
  std::ostream& operator << (std::ostream& os, 
                             const google::protobuf::Message& message);
  // 从line里读取
  template <class T>
  std::ostream& operator << (std::ostream& os,
                             const std::vector<T>& data) {
    for (auto it = data.begin(); it != data.end(); it++) {
      os << *it << std::endl;
    }
    return true;
  }

  // 从文件里读取
  template <class T>
  bool WriteToIO(const IOConfig& config,
                 const std::vector<T>& data) {
    switch (config.store_method()) {
      case LevelDb:
        break;
      case SSTable:
        break;
      case File:
        break;
      case Mysql:
        return WriteToMysql(config, data);
        break;
      default:
        return false;
    }
    return true;
  }

  // 从文件里读取
  template <class T>
  bool WriteToMysql(const IOConfig& config,
                    const std::vector<T>& data) {
    MysqlHandler mysql_handler;
    mysql_handler.Open(config.mysql_config());
    mysql_handler.BuildTable<T>();
    std::for_each(data.begin(), data.end(),
        std::bind(&MysqlHandler::Insert, mysql_handler, std::placeholders::_1));
    return true;
  }



};
}  // namespace gdt

#endif  // COMMON_WRITER_WRITER_H_
