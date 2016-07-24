// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

// 对本地数据库的操作的类
#ifndef CREATIVE_DYNAMIC_CREATIVE_DATABASE_LOCAL_DB_OPERATOR_H_
#define CREATIVE_DYNAMIC_CREATIVE_DATABASE_LOCAL_DB_OPERATOR_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <bitset>

#include "thirdparty/google/protobuf/text_format.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "creative/dynamic_creative/common/leveldb.h"
#include "creative/dynamic_creative/proto/commodity.pb.h"
#include "creative/dynamic_creative/proto/commodity_template.pb.h"
#include "creative/dynamic_creative/proto/dynamic_creative_config.pb.h"
#include "creative/dynamic_creative/database/select_conditions.h"

namespace gdt {
namespace dynamic_creative {

typedef shared_ptr< ::leveldb::Iterator> CursorType;

// 对本地数据库的操作的类
template <class T, uint64_t (T::*GetKey)() const>
class LocalDbOperator {
 public:
  // 插入
  bool Insert(const T& proto) {
    std::string value;
    if (proto.SerializeToString(&value)) {
      return database_.Insert(ConvertToString((proto.*GetKey)()), value);   
    } else {
      return false;
    }
  }
  
  // 是否变化
  bool Changed(const T& proto) {
    std::string value;
    if (!database_.Get(ConvertToString((proto.*GetKey)()), &value)) {
      return false;
    } else {
      std::string origin_value;
      if (proto.SerializeToString(&origin_value) && origin_value == value) {
        return false;
      }
      return true;
    }
  }

  // 获取
  bool Get(uint64_t key, T* proto) {
    std::string value;
    if (!database_.Get(ConvertToString(key), &value)) {
      return false;
    } else {
      if (proto->ParseFromString(value)) {
        return true;
      } else {
        return false;
      }
    }
  }

  // 是否存在
  bool Existing(uint64_t key) {
    std::string value;
    return database_.Get(ConvertToString(key), &value);
  }

  // 是否存在
  template <class ValueType, ValueType (T::*GetValue)() const>
  bool Check(uint64_t key, ValueType value) {
    T proto;
    if (!Get(key, &proto)) {
      return false;
    }
    if ((proto.*GetValue)() != value) {
      return false;
    }
    return true;
  }

  bool Open(const std::string file) {
    return database_.Open(file);
  }

  bool Close() {
    database_.Close();
    return true;
  }

  // 载入全量
  bool Load(std::vector<T>* protos) {
    if (!database_.IsOpened()) {
      return false;
    }
    CursorType cursor(database_.Begin());
    if (!cursor) {
      return false;
    }
    while (cursor->Valid()) {
      std::string key, value;
      if (database_.Fetch(&cursor, &key, &value)) {
        T proto;
        if (proto.ParseFromString(value)) {
          (*protos).push_back(proto);
        } else {
          LOG(ERROR) << "ParseFromString Failed ";
        }
      }
      database_.IterNext(&cursor);
      if (!cursor) {
        break;
      }
    }
    return true;
  }

  // 插入全量
  bool Insert(const std::vector<T>& protos) {
    if (!database_.IsOpened()) {
      return false;
    }
    for (typename std::vector<T>::const_iterator iter = protos.begin();
        iter != protos.end(); iter++) {
      Insert(*iter);
    }
    return true;    
  }

  // 带条件插入
  template <class T2 , class T3>
  bool Insert(const std::vector<T>& protos, const SelectConditions<T, T2, T3>& conditions) {
    if (!database_.IsOpened()) {
      return false;
    }
    for (typename std::vector<T>::const_iterator iter = protos.begin();
        iter != protos.end(); iter++) {
      if (conditions.Satisfy(*iter)) {
        Insert(*iter);
      }
    }
    return true;    
  }

  uint64_t Size() {
    return database_.Size();
  }

 private:
  // 数据库
  Table<NaiveComparator> database_;
};

}  //  namespace dynamic_creative
}  //  namespace gdt

#endif  // CREATIVE_DYNAMIC_CREATIVE_DATABASE_LOCAL_DB_OPERATOR_H_
