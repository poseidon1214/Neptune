// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Qian Wang <cernwang@tencent.com>

#include "data_storer/sql/mysql_handler.h"

#include <google/protobuf/text_format.h>
#include <string>
#include <time.h>
#include <vector>
#include <typeinfo>
#include <sstream>
#include "app/qzap/common/base/string_utility.h"
#include "common/base/string/string_number.h"
#include "common/encoding/percent.h"
#include "thirdparty/jsoncpp/reader.h"
#include "thirdparty/jsoncpp/value.h"
#include "thirdparty/jsoncpp/writer.h"
#include "thirdparty/protobuf/descriptor.h"
#include "thirdparty/protobuf/message.h"

using namespace std;  // NOLINT(build/namespaces)
using namespace google::protobuf;  // NOLINT(build/namespaces)

namespace gdt {
template <class T>
static std::string ConvertToString(const T& t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}

static std::string ConvertBoolToString(const bool& a) {
  return a?"true":"false";
}

static std::string ConvertFloatToString(const float& a) {
  return ConvertToString(double(a));
}

bool MysqlHandler::BuildTable(const google::protobuf::Message& record) {
  std::map<std::string, std::string> parameters;
   std::vector<std::string> primary_key_list;
  if (!ProtoMessageToTypeMap(record, &parameters, &primary_key_list)) {
    return false;
  }

  std::string table = record.GetDescriptor()->name();
  std::string sql = GenerateBuildSql(table, parameters, primary_key_list);
  LOG(ERROR) << sql;
  std::vector<std::map<std::string, std::string> > results;
  return Exec(sql, &results);
}
// 
bool MysqlHandler::Insert(const google::protobuf::Message& record) {
  std::string error_msg;
  std::map<std::string, std::string> parameters;
  if (!ProtoMessageToMap(record, &parameters, &error_msg)) {
    LOG(ERROR) << "Error:" << error_msg ;
    return false;
  }
  std::string table = record.GetDescriptor()->name();
  std::string sql = GenerateInsertSql(table, parameters);
  std::vector<std::map<std::string, std::string> > results;
  return Exec(sql, &results);
}

bool MysqlHandler::Open(const MysqlConfig& mysql_config) {
  mysql = mysql_init(NULL);
  if (!mysql) {
  LOG(ERROR) << "Mysql init failed";
  return false;
  }
  mysql_options(mysql, MYSQL_SET_CHARSET_NAME, mysql_config.charset().c_str());
  mysql = mysql_real_connect(mysql, mysql_config.ip().c_str(), mysql_config.user().c_str(),
    mysql_config.password().c_str(), mysql_config.database_name().c_str(), mysql_config.port(), NULL, 0);
  if (mysql) {
  LOG(INFO) << "Connection success!";
  return true;
  } else {
  LOG(ERROR) << "Connection failed!";
  return false;
  }
}
 
void MysqlHandler::Close() {
  mysql_close(mysql);
}

bool MysqlHandler::FetchRows(MYSQL* db, std::vector< std::map<std::string, std::string> >* rows) {
  bool success = true;
  int status = 0;
  while (status == 0) {
  MYSQL_RES* result = mysql_store_result(db);
  if (result) {
    int num_cols = mysql_num_fields(result);
    std::vector<std::string> col_nums(num_cols);
    for (int i=0; i<num_cols; i++) {
    MYSQL_FIELD* field = mysql_fetch_field_direct(result, i);
    col_nums[i] = field->name;
    }
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
    std::map<std::string, std::string> map;
    for (int i = 0; i < num_cols; i++) {
      if(!row[i]) {
      map.insert(std::make_pair(col_nums[i], "null"));
      } else {
      map.insert(std::make_pair(col_nums[i], row[i]));
      }
    }
    rows->push_back(map);
    }
    mysql_free_result(result);
  } else {
    if (mysql_field_count(db)) {
    success = false;
    break;
    }
  }
  if ((status = mysql_next_result(db)) > 0) {
    success = false;
    break;
  }
  }
  return success;
}


std::string MysqlHandler::GenerateInsertSql(const std::string& table, 
  const std::map<std::string, std::string>& parameters) {
  return GenerateUpdateSql(table, parameters);
}

std::string MysqlHandler::GenerateUpdateSql(const std::string& table, 
  const std::map<std::string, std::string>& parameters) {
  std::string sql;
  sql += "REPLACE INTO `" + table + "` (";
  bool first = true;
  for (std::map<std::string, std::string>::const_iterator iter = parameters.begin(); 
    iter != parameters.end(); iter++) {
  if (!first) {
    sql +=  ",";
  } else {
    first = false;
  }
  sql += iter->first;
  }
  sql += ") VALUES (";
  first = true;
  for (std::map<std::string, std::string>::const_iterator iter = parameters.begin(); 
    iter != parameters.end(); iter++) {
  if (!first) {
    sql +=  ",";
  } else {
    first = false;
  }
  sql += "'" + iter->second + "'";
  }
  sql += ");";
  return sql;
}


bool MysqlHandler::Exec(const std::string& sql, std::vector<std::map<std::string, std::string> >* results) {
  bool ret = true;
  MYSQL* db = mysql;
  if (db == NULL) {
  LOG(ERROR) << " No available connection!";
  return false;
  }
  if (mysql_real_query(db, sql.c_str(), sql.length()) > 0 && mysql_errno(db)) {
  LOG(ERROR) << "Mysql Query Failed:" << mysql_error(db);
  LOG(ERROR) << "Sql:" << sql;
  ret = false;
  } else {
  ret = FetchRows(db, results);
  }
  return ret;
}

bool MysqlHandler::ProtoMessageToMap(
  const google::protobuf::Message& message,
  std::map<std::string, std::string>* parameters,
  std::string* error) {

  if (!message.IsInitialized()) {
  *error = message.InitializationErrorString();
  return false;
  }

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
#undef CASE_FIELD_TYPE

#define CASE_BOOL_FIELD_TYPE(cpptype, method) \
    case FieldDescriptor::CPPTYPE_##cpptype: { \
    if (field->is_repeated()) { \
      int field_size = reflection->FieldSize(message, field); \
      for (int index = 0; index < field_size; index++) { \
      (*parameters)[field->name()] += \
                ConvertBoolToString( \
                  reflection->GetRepeated##method( \
                    message, field, index)) + ";"; \
      } \
    }  else { \
      (*parameters)[field->name()] = ConvertBoolToString( \
              reflection->Get##method( \
                message, field)); \
    } \
    break; \
    }

    CASE_BOOL_FIELD_TYPE(BOOL, Bool);
#undef CASE_BOOL_FIELD_TYPE

#define CASE_FLOAT_FIELD_TYPE(cpptype, method) \
    case FieldDescriptor::CPPTYPE_##cpptype: { \
    if (field->is_repeated()) { \
      int field_size = reflection->FieldSize(message, field); \
      for (int index = 0; index < field_size; index++) { \
      (*parameters)[field->name()] += \
                ConvertFloatToString( \
                  reflection->GetRepeated##method( \
                    message, field, index)) + ";"; \
      } \
    }  else { \
      (*parameters)[field->name()] = ConvertFloatToString( \
              reflection->Get##method( \
                message, field)); \
    } \
    break; \
    }

    CASE_FLOAT_FIELD_TYPE(FLOAT,  Float);
#undef CASE_FLOAT_FIELD_TYPE

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

bool MysqlHandler::ProtoMessageToTypeMap(
  const google::protobuf::Message& message,
  std::map<std::string, std::string>* parameters,
  std::vector<std::string>* primary_key_list) {

  using namespace std;  // NOLINT(build/namespaces)
  using namespace google::protobuf;  // NOLINT(build/namespaces)

  const Reflection* reflection = message.GetReflection();
  const Descriptor* descriptor = message.GetDescriptor();

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
    if (field->is_repeated()) {
      continue;
    }
    if (field->number() == 1) {
      primary_key_list->push_back(field->name());
    }
    switch (field->cpp_type()) {
#define CASE_FIELD_TYPE(cpptype, sql_type) \
      case FieldDescriptor::CPPTYPE_##cpptype: { \
        (*parameters)[field->name()] = #sql_type; \
        break; \
      }
      CASE_FIELD_TYPE(INT32,  BIGINT(64));
      CASE_FIELD_TYPE(UINT32, BIGINT(64));
      CASE_FIELD_TYPE(DOUBLE, FLOAT);
      CASE_FIELD_TYPE(INT64,  BIGINT(64));
      CASE_FIELD_TYPE(UINT64, BIGINT(64));
      CASE_FIELD_TYPE(BOOL, BIGINT(64));
      CASE_FIELD_TYPE(FLOAT, FLOAT);
      CASE_FIELD_TYPE(STRING, BLOB);
      CASE_FIELD_TYPE(ENUM, BIGINT(64));
      CASE_FIELD_TYPE(MESSAGE, BLOB);
    }
  }
  return true;
}

//
std::string MysqlHandler::GenerateBuildSql(const std::string& table, 
    const std::map<std::string, std::string>& parameters,
    const std::vector<std::string>& primary_key_list) {
  std::string sql = "CREATE TABLE IF NOT EXISTS `" + table + "` (\n";
  for (auto parameter : parameters) {
    sql += "`" + parameter.first + "`  " + parameter.second + ",\n";
  }
  bool first = true;
  if (!primary_key_list.empty()) {
    sql += "PRIMARY KEY(";
    for (auto key : primary_key_list) {
      if (!first) {
        sql +=  ",";
      } else {
        first = false;
      }
      sql += "`" + key + "`" ;
    }
    sql += ")\n";    
  } 
  sql += ") ENGINE=InnoDB  DEFAULT CHARSET=utf8;" ;
  return sql;
}

}  // namespace gdt
