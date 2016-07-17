// Copyright (c) 2012, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 03/05/12
// Description: HttpRequest implementation

#include "common/net/http/http_request.h"
#include <string>
#include <vector>
#include "common/base/string/algorithm.h"
#include "common/base/string/concat.h"
#include "thirdparty/glog/logging.h"

namespace gdt {

// NOTE: The order must be consistent with enum values because GetMethodName
// access this table by method_type enum as index
static const struct {
  int method;
  const char* method_name;
} kValidMethodNames[] = {
  { HttpRequest::METHOD_HEAD, "HEAD" },
  { HttpRequest::METHOD_GET, "GET" },
  { HttpRequest::METHOD_POST, "POST" },
  { HttpRequest::METHOD_PUT, "PUT" },
  { HttpRequest::METHOD_DELETE, "DELETE" },
  { HttpRequest::METHOD_OPTIONS, "OPTIONS" },
  { HttpRequest::METHOD_TRACE, "TRACE" },
  { HttpRequest::METHOD_CONNECT, "CONNECT" },
  { HttpRequest::METHOD_UNKNOWN, NULL },
};

void HttpRequest::Reset() {
  HttpMessage::Reset();
  m_method = METHOD_UNKNOWN;
  m_uri = "/";
}

// static
int HttpRequest::GetMethodByName(StringPiece method_name) {
  for (int i = 0; ; ++i) {
    if (kValidMethodNames[i].method_name == NULL) {
      return HttpRequest::METHOD_UNKNOWN;
    }
    // Method is case sensitive.
    if (method_name == kValidMethodNames[i].method_name) {
      return kValidMethodNames[i].method;
    }
  }
}

// static
const char* HttpRequest::GetMethodName(int method) {
  if (method <= METHOD_UNKNOWN || method >= METHOD_UPPER_BOUND) {
    return NULL;
  }
  return kValidMethodNames[method].method_name;
}

bool HttpRequest::ParseStartLine(const StringPiece& data,
                                 HttpMessage::ErrorType* error) {
  ErrorType error_placeholder;
  if (error == NULL)
    error = &error_placeholder;

  static const size_t kMinHttpMethodLength = 3;
  size_t pos = data.find(' ', kMinHttpMethodLength);
  if (pos == StringPiece::npos) {
    *error = ERROR_START_LINE_NOT_COMPLETE;
    return false;
  }

  StringPiece method = data.substr(0, pos);
  StringTrim(&method);
  m_method = GetMethodByName(method);
  if (m_method == METHOD_UNKNOWN) {
    *error = ERROR_METHOD_NOT_FOUND;
    return false;
  }

  size_t prev_pos = pos + 1;
  pos = data.find(' ', prev_pos);
  StringPiece uri;
  if (pos == StringPiece::npos) {
    uri = data.substr(prev_pos);
  } else {
    uri = data.substr(prev_pos, pos - prev_pos);
  }
  StringTrim(&uri);
  uri.copy_to_string(&m_uri);

  if (pos != StringPiece::npos) {
    StringPiece version = data.substr(pos);
    StringTrim(&version);
    int http_version = GetVersionNumber(version);
    if (http_version == HttpMessage::VERSION_UNKNOWN) {
      *error = ERROR_VERSION_UNSUPPORTED;
      return false;
    }
    set_http_version(http_version);
  }

  return true;
}

void HttpRequest::AppendStartLineToString(std::string* result) const {
  CHECK_NE(m_method, METHOD_UNKNOWN);
  StringAppend(result,
               GetMethodName(m_method),
               " ",
               m_uri,
               " ",
               GetVersionString(http_version()));
}

}  // namespace gdt

