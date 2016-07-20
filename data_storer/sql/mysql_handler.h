// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Qian Wang <cernwang@tencent.com>

#ifndef CREATIVE_DYNAMIC_CREATIVE_TOOL_GOLDENEYE_SRC_MYSQL_HANDLER_H_
#define CREATIVE_DYNAMIC_CREATIVE_TOOL_GOLDENEYE_SRC_MYSQL_HANDLER_H_

#include <string>
#include <list>
#include "thirdparty/glog/logging.h"
#include "thirdparty/mysql/mysql.h"
#include "data_storer/sql/proto/mysql_config.pb.h"

namespace gdt {

class MysqlHandler {
 public:
  //
  MysqlHandler(): mysql(NULL) {}
  //
  bool Open(const MysqlConfig& mysql_config);
  //
  void Close();
  // 
  template <class T>
  bool BuildTable() {
    return BuildTable(T());
  }
  // 
  bool BuildTable(const google::protobuf::Message& record);
  // 
  bool Insert(const google::protobuf::Message& record);
  // 
  template <typename Record>
  bool Delete(const Record& record);
  //
  template <typename Record>
  bool Update(const Record& record);
  //
  template <typename Record>
  bool Select(const Lambda& lambda, std::vector<Record>* records) {
    std::string sql = GenerateSelectSql(Record()) + GenerateSelectSql(lambda);
    LOG(ERROR) << sql;
    std::vector<std::map<std::string, std::string> > results;
    if (!Exec(sql, &results)) {
      return false;
    }
    for (auto result : results) {
      Record record;
      if (MapToProtoMessage(result, &record)) {
        records->push_back(record);
      }
    }
    return true;
  }

 public:
  //
  bool FetchRows(MYSQL* db, std::vector< std::map<std::string, std::string> >* rows);
  //
  bool Exec(const std::string& sql, std::vector<std::map<std::string, std::string> >* results);
  //
  bool ProtoMessageToTypeMap(
      const google::protobuf::Message& message,
      std::map<std::string, std::string>* paramenters,
      std::vector<std::string>* primary_key_list);
  //
  std::string GenerateInsertSql(const std::string& table, 
      const std::map<std::string, std::string>& parameters);
  //
  std::string GenerateUpdateSql(const std::string& table, 
      const std::map<std::string, std::string>& parameters);
  //
  std::string GenerateBuildSql(const std::string& table, 
      const std::map<std::string, std::string>& parameters,
      const std::vector<std::string>& primary_key_list);
  //
  std::string GenerateConditionSql(const Lambda& lambda);
  //
  std::string GenerateConditionSql(const Expression& expression);
  //
  std::string GenerateSelectSql(const google::protobuf::Message& record);

 private:
  // 数据库句柄
  MYSQL* mysql;
};

}  // namespace gdt

#endif  // CREATIVE_DYNAMIC_CREATIVE_TOOL_GOLDENEYE_SRC_MYSQL_HANDLER_H_
