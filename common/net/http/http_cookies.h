// Copyright (c) 2015 Tencent Inc.
// Author: Sun Naicai (naicaisun@tencent.com)

#ifndef COMMON_NET_HTTP_HTTP_COOKIES_H_
#define COMMON_NET_HTTP_HTTP_COOKIES_H_

#include <list>
#include <string>

namespace gdt {

class HttpRequest;

class HttpCookie {
 public:
  HttpCookie(const std::string& name, const std::string& value)
      : name_(name), value_(value) {
  }
  // Parse http-header: Cookie or Cookie2
  static void ParseCookie(const std::string& cookie_str,
                          std::list<HttpCookie>* cookies);
  static void ParseCookieFromRequest(const HttpRequest& http_request,
                                     std::list<HttpCookie>* cookies);
  // TODO(naicaisun) Parse http-header: SetCookie
  const std::string& name() const {
    return name_;
  }

  const std::string& value() const {
    return value_;
  }
  void set_value(const std::string& value) {
    value_ = value;
  }

  const std::string& domain() const {
    return domain_;
  }
  void set_domain(const std::string& domain) {
    domain_ = domain;
  }

  const std::string& path() const {
    return path_;
  }
  void set_path(const std::string& path) {
    path_ = path;
  }

 private:
  std::string name_;
  std::string value_;
  std::string domain_;  // reserved
  std::string path_;  // reserved
  // bool secure_;  // reserved
  // bool http_only_;  // reserved
  // uint32_t version_;  // default = 0
  // uint64_t max_age_;  // the lifetime of the cookie in seconds
};

class HttpCookies {
 public:
  void ParseFromCookie(const std::string& cookie_str);
  void ParseFromRequest(const HttpRequest& http_request);
  HttpCookie* Find(const std::string& name);
  const HttpCookie* Find(const std::string& name) const;
  bool GetValue(const std::string& name, std::string* value) const;

  void Clear();
  bool IsEmpty() const;

  void Set(const HttpCookie& cookie);
  void Set(const std::string& name, const std::string& value);

  void Add(const HttpCookie& cookie);
  void Add(const std::string& name, const std::string& value);

  bool Remove(const std::string& name);
 private:
  std::list<HttpCookie> cookies_;
};

}  // namespace gdt

#endif  // COMMON_NET_HTTP_HTTP_COOKIES_H_
