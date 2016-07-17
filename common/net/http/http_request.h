// Copyright (c) 2012, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 03/05/12
// Description: HttpRequest class declaration

#ifndef COMMON_NET_HTTP_HTTP_REQUEST_H_
#define COMMON_NET_HTTP_HTTP_REQUEST_H_
#pragma once

#include <algorithm>
#include <string>
#include "common/net/http/http_message.h"

namespace gdt {

// Describes a http request.
class HttpRequest : public HttpMessage {
 public:
  enum MethodType {
    METHOD_UNKNOWN = -1,
    METHOD_HEAD,
    METHOD_GET,
    METHOD_POST,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_OPTIONS,
    METHOD_TRACE,
    METHOD_CONNECT,
    METHOD_UPPER_BOUND,  // no use, just label the bound.
  };

  HttpRequest() : m_method(METHOD_UNKNOWN), m_uri("/") {
  }
  ~HttpRequest() {}
  virtual void Reset();

 public:
  static int GetMethodByName(StringPiece method_name);
  static const char* GetMethodName(int method);

  int method() const {
    return m_method;
  }
  void set_method(int method) {
    m_method = method;
  }

  const std::string& uri() const {
    return m_uri;
  }
  void set_uri(const std::string& uri) {
    m_uri = uri;
  }

  void Swap(HttpRequest* other) {
    HttpMessage::Swap(other);
    using std::swap;
    swap(m_method, other->m_method);
    swap(m_uri, other->m_uri);
  }

 private:
  virtual void AppendStartLineToString(std::string* result) const;
  virtual bool ParseStartLine(const StringPiece& data, ErrorType* error = NULL);

  int m_method;
  std::string m_uri;
};

}  // namespace gdt

// adapt to std::swap
namespace std {

template <>
inline void swap(gdt::HttpRequest& lhs, gdt::HttpRequest& rhs) {
  lhs.Swap(&rhs);
}

}  // namespace std

#endif  // COMMON_NET_HTTP_HTTP_REQUEST_H_
