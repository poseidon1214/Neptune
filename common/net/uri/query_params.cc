// Copyright (c) 2011, Tencent.com
// All rights reserved.
//
// Author CHEN Feng <phongchen@tencent.com>
// Date  03/30/2011 10:58:53 PM

#include "common/net/uri/query_params.h"

#include <stddef.h>
#include "common/base/string/string_number.h"
#include "common/encoding/percent.h"
#include "common/net/http/http_request.h"
#include "common/net/uri/uri.h"

namespace gdt {

bool QueryParams::Parse(const std::string& params) {
  Clear();
  return ParsePartial(params);
}

bool QueryParams::ParseFromUrl(const std::string& url) {
  net::URI uri;
  if (!uri.Parse(url) || !uri.HasQuery())
    return false;
  return Parse(uri.Query());
}

bool QueryParams::ParseFromRequest(const HttpRequest* request) {
  net::URI uri;
  if (uri.Parse(request->uri()) && uri.HasQuery()) {
    if (!ParsePartial(uri.Query()))
      return false;
  }
  if (request->method() == HttpRequest::METHOD_POST) {
    std::string content_type = request->GetHeader("Content-Type");
    if (ContainsSubString(content_type, "application/x-www-form-urlencoded")) {
      if (!ParsePartial(request->body()))
        return false;
    }
  }
  return true;
}

bool QueryParams::ParsePartial(const std::string& params) {
  std::vector<std::string> splited;
  SplitString(params, "&", &splited);
  for (size_t i = 0; i < splited.size(); ++i) {
    m_params.push_back(QueryParam());
    size_t pos = splited[i].find('=');
    if (pos != std::string::npos) {
      m_params.back().name.assign(splited[i], 0, pos);
      m_params.back().value.assign(splited[i], pos + 1, std::string::npos);
      if (!PercentEncoding::Decode(&m_params.back().value))
        return false;
    } else {
      m_params.back().name = splited[i];
    }
  }
  return true;
}

void QueryParams::AppendToString(std::string* target) const {
  for (size_t i = 0; i < m_params.size(); ++i) {
    if (!m_params[i].name.empty()) {
      const QueryParam& param = m_params[i];
      target->append(param.name);
      target->push_back('=');
      target->append(PercentEncoding::Encode(param.value));
      if (i != m_params.size() - 1)
        target->push_back('&');
    }
  }
}

void QueryParams::WriteToString(std::string* target) const {
  target->clear();
  AppendToString(target);
}

std::string QueryParams::ToString() const {
  std::string result;
  AppendToString(&result);
  return result;
}

QueryParam* QueryParams::Find(const std::string& name) {
  for (size_t i = 0; i < m_params.size(); ++i) {
    if (m_params[i].name == name)
      return &m_params[i];
  }
  return NULL;
}

const QueryParam* QueryParams::Find(const std::string& name) const {
  return const_cast<QueryParams*>(this)->Find(name);
}

QueryParam& QueryParams::Get(size_t index) {
  return m_params.at(index);
}

const QueryParam& QueryParams::Get(size_t index) const {
  return m_params.at(index);
}

int32_t QueryParams::GetAsInt32(
    const std::string& name,
    int32_t default_value) const {
  int32_t value;
  if (GetValue(name, &value))
    return value;
  return default_value;
}

int64_t QueryParams::GetAsInt64(
    const std::string& name,
    int64_t default_value) const {
  int64_t value;
  if (GetValue(name, &value))
    return value;
  return default_value;
}

const std::string& QueryParams::GetOrDefaultValue(
    const std::string& name,
    const std::string& default_value) const {
  const QueryParam* param = Find(name);
  if (param) {
    return param->value;
  }
  return default_value;
}

size_t QueryParams::Count() const {
  return m_params.size();
}

void QueryParams::Clear() {
  m_params.clear();
}

bool QueryParams::IsEmpty() const {
  return m_params.empty();
}

void QueryParams::Add(const QueryParam& param) {
  m_params.push_back(param);
}

void QueryParams::Add(const std::string& name, const std::string& value) {
  m_params.push_back(QueryParam());
  m_params.back().name = name;
  m_params.back().value = value;
}

void QueryParams::Set(const std::string& name, const std::string& value) {
  QueryParam* param = Find(name);
  if (param)
    param->value = value;
  else
    Add(name, value);
}

void QueryParams::Add(const std::string& name, int64_t value) {
  Add(name, ConvertToString(value));
}

void QueryParams::Set(const std::string& name, int64_t value) {
  Set(name, ConvertToString(value));
}

bool QueryParams::Remove(const std::string& name) {
  std::vector<QueryParam>::iterator iter;
  for (iter = m_params.begin(); iter != m_params.end(); ++iter) {
    if ((*iter).name == name) {
      m_params.erase(iter);
      return true;
    }
  }
  return false;
}

}  // namespace gdt
