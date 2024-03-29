// Copyright (c) 2012, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 01/12/12
// Description: HttpHeaders class

#ifndef COMMON_NET_HTTP_HTTP_HEADERS_H
#define COMMON_NET_HTTP_HTTP_HEADERS_H
#pragma once

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "common/base/string/string_piece.h"

namespace gdt {

// Store http headers information
class HttpHeaders {
 public:
  // Return false if it doesn't exist.
  bool Get(const StringPiece& header_name, std::string** value);
  bool Get(const StringPiece& header_name, const std::string** value) const;
  bool Get(const StringPiece& header_name, std::string* value) const;

  // Used when a http header appears multiple times.
  // return false if it doesn't exist.
  bool Get(const StringPiece& header_name,
           std::vector<std::string>* header_values) const;
  // Set a header field. if it exists, overwrite the header value.
  HttpHeaders& Set(const StringPiece& header_name,
                   const StringPiece& header_value);
  // Add a header field, just append, no overwrite.
  HttpHeaders& Add(const StringPiece& header_name,
                   const StringPiece& header_value);
  // Add all the header fields in rhs into this. no overwrite.
  HttpHeaders& Add(const HttpHeaders& rhs);

  // Remove an http header field.
  bool Remove(const StringPiece& header_name);

  // Get count of header
  size_t Count() const;

  // Get header by index
  bool GetAt(int index, std::pair<std::string, std::string>* header) const;

  // If has a header
  bool Has(const StringPiece& header_name) const;

  // Convert start line and headers to string.
  void AppendToString(std::string* result) const;
  void ToString(std::string* result) const;
  std::string ToString() const;

  bool Parse(const StringPiece& data, int* error = NULL);

  void Clear();

  void Swap(HttpHeaders* rhs);

 private:
  bool ParseLine(const StringPiece& line, int* error);
  std::vector<std::pair<std::string, std::string> > m_headers;
};

} // namespace gdt

// adapt to std::swap
namespace std {
template <>
inline void swap(gdt::HttpHeaders& lhs, gdt::HttpHeaders& rhs) {
  lhs.Swap(&rhs);
}
} // namespace std

#endif // COMMON_NET_HTTP_HTTP_HEADERS_H
