// Copyright (c) 2011, Tencent.com
// All rights reserved.
//
// Author CHEN Feng <phongchen@tencent.com>
// Date  03/30/2011 10:58:53 PM

#ifndef COMMON_NET_URI_QUERY_PARAMS_H_
#define COMMON_NET_URI_QUERY_PARAMS_H_

#include <stdint.h>
#include <string>
#include <vector>
#include "common/base/string/algorithm.h"
#include "common/base/string/string_piece.h"

namespace gdt {

class HttpRequest;

struct QueryParam {
 public:
  QueryParam() {}
  QueryParam(const std::string& aname, const std::string& avalue)
    : name(aname), value(avalue) {
  }
 public:
  std::string name;
  std::string value;
};

class QueryParams {
 public:
  bool Parse(const std::string& params);
  bool ParseFromUrl(const std::string& url);
  bool ParseFromRequest(const HttpRequest* request);
  void AppendToString(std::string* target) const;
  void WriteToString(std::string* target) const;
  std::string ToString() const;

  const QueryParam* Find(const std::string& name) const;
  QueryParam* Find(const std::string& name);

  QueryParam& Get(size_t index);
  const QueryParam& Get(size_t index) const;

  template<class T>
  bool GetValue(const std::string& name, T* value) const;

  const std::string& GetOrDefaultValue(
    const std::string& name,
    const std::string& default_value) const;
  int32_t GetAsInt32(const std::string& name, int32_t default_value = 0) const;
  int64_t GetAsInt64(const std::string& name, int64_t default_value = 0) const;

  size_t Count() const;
  void Clear();
  bool IsEmpty() const;
  void Add(const QueryParam& param);
  void Add(const std::string& name, const std::string& value);
  void Set(const std::string& name, const std::string& value);
  void Add(const std::string& name, int64_t value);
  void Set(const std::string& name, int64_t value);
  bool Remove(const std::string& name);

 private:
  bool ParsePartial(const std::string& query);

 private:
  std::vector<QueryParam> m_params;
};

// 整型数或浮点数
template<class T>
bool QueryParams::GetValue(const std::string& name, T* value) const {
  const QueryParam* param = Find(name);
  if (param) {
    return StringToNumeric(param->value, value);
  }
  return false;
}

// 模板特化:string类型
template<>
inline bool QueryParams::GetValue<std::string>(const std::string& name,
                                               std::string* value) const {
  const QueryParam* param = Find(name);
  if (param) {
    *value = param->value;
    return true;
  }
  return false;
}

}  // namespace gdt

#endif  // COMMON_NET_URI_QUERY_PARAMS_H_

