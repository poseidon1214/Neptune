// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: YU Shizhan <stanyu@tencent.com>
// Created: 09/07/11
// Description:

#ifndef COMMON_NET_MIME_MIME_HPP
#define COMMON_NET_MIME_MIME_HPP
#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include "common/system/concurrency/once.h"

namespace gdt {

class MimeType {
 public:
  MimeType() {}

  MimeType(const std::string& type, const std::string& subtype)
    : m_type(type), m_subtype(subtype) {}

  explicit MimeType(const std::string& mime) {
    if (!Set(mime))
      throw std::runtime_error("Invalid MIME: " + mime);
  }

  void Set(const std::string& type, const std::string& subtype) {
    m_type = type;
    m_subtype = subtype;
  }

  bool Set(const std::string& mime);

  const std::string& Type() const {
    return m_type;
  }

  const std::string& SubType() const {
    return m_subtype;
  }

  bool Match(const MimeType& mime) const;

  bool Match(const std::string& mime) const;

  bool Empty() {
    return (m_type.empty() && m_subtype.empty());
  }
  /// convert mime to string
  std::string ToString() const {
    return m_type + "/" + m_subtype;
  }

 public:
  // .xml -> text/xml
  //  /etc/mime.types
  static MimeType FromFileExtension(const std::string& ext);

 private:
  typedef std::map<std::string, std::string> MapType;
  // Initialize mime map
  static MapType& GetMap();
  static MapType& DoGetMap();

  std::string m_type;
  std::string m_subtype;
  static Once s_once;
};

}
#endif // COMMON_NET_MIME_MIME_HPP
