// Copyright (c) 2015 Tencent Inc.
// Author: Sun Naicai (naicaisun@tencent.com)

#include "common/net/http/http_cookies.h"

#include <ctype.h>
#include <vector>

#include "common/base/string/algorithm.h"
#include "common/net/http/http_request.h"

namespace gdt {

void HttpCookie::ParseCookie(const std::string& cookie_str,
                             std::list<HttpCookie>* cookies) {
  if (cookie_str.empty() || cookies == NULL) {
    return;
  }
  // only support delimiter semicolon currently, may need support comma
  std::vector<std::string> attr_value_list;
  SplitString(cookie_str, ";", &attr_value_list);
  if (attr_value_list.empty()) {
    // perhaps only one cookie
    attr_value_list.push_back(cookie_str);
  }
  for (size_t i = 0; i < attr_value_list.size(); ++i) {
    const std::string& attr_value = attr_value_list.at(i);
    if (attr_value.empty()) {
      continue;
    }
    size_t equal_sign_pos = attr_value.find("=");
    if (equal_sign_pos == std::string::npos) {
      continue;
    }
    std::string attribute(attr_value, 0, equal_sign_pos);
    std::string value(attr_value, equal_sign_pos + 1, std::string::npos);
    StringTrim(&attribute);
    StringTrim(&value);
    if (!attribute.empty() && isalpha(attribute.at(0)) && !value.empty()) {
      cookies->push_back(HttpCookie(attribute, value));
    }
  }
}

void HttpCookie::ParseCookieFromRequest(const HttpRequest& http_request,
                                        std::list<HttpCookie>* cookies) {
  std::vector<std::string> cookie_sequence;
  http_request.GetHeaders("Cookie", &cookie_sequence);
  for (std::vector<std::string>::const_iterator iter_cookie =
       cookie_sequence.begin();
       iter_cookie != cookie_sequence.end(); ++iter_cookie) {
    ParseCookie(*iter_cookie, cookies);
  }
}

void HttpCookies::ParseFromCookie(const std::string& cookie_str) {
  HttpCookie::ParseCookie(cookie_str, &cookies_);
}

void HttpCookies::ParseFromRequest(const HttpRequest& http_request) {
  HttpCookie::ParseCookieFromRequest(http_request, &cookies_);
}

HttpCookie* HttpCookies::Find(const std::string& name) {
  for (std::list<HttpCookie>::iterator iter = cookies_.begin();
       iter != cookies_.end(); ++iter) {
    if (iter->name() == name) {
      return &(*iter);
    }
  }

  return NULL;
}

const HttpCookie* HttpCookies::Find(const std::string& name) const {
  return const_cast<HttpCookies*>(this)->Find(name);
}

bool HttpCookies::GetValue(const std::string& name, std::string* value) const {
  const HttpCookie* cookie = Find(name);
  if (cookie != NULL) {
    *value = cookie->value();
    return true;
  }

  return false;
}

void HttpCookies::Clear() {
  cookies_.clear();
}

bool HttpCookies::IsEmpty() const {
  return cookies_.empty();
}

void HttpCookies::Set(const HttpCookie& cookie) {
  HttpCookie* http_cookie = Find(cookie.name());
  if (http_cookie != NULL) {
    http_cookie->set_value(cookie.value());
  } else {
    Add(cookie);
  }
}

void HttpCookies::Set(const std::string& name, const std::string& value) {
  HttpCookie* http_cookie = Find(name);
  if (http_cookie != NULL) {
    http_cookie->set_value(value);
  } else {
    Add(name, value);
  }
}

void HttpCookies::Add(const HttpCookie& cookie) {
  cookies_.push_back(cookie);
}

void HttpCookies::Add(const std::string& name, const std::string& value) {
  cookies_.push_back(HttpCookie(name, value));
}

bool HttpCookies::Remove(const std::string& name) {
  bool erased = false;
  for (std::list<HttpCookie>::iterator iter = cookies_.begin();
       iter != cookies_.end();) {
    if (iter->name() == name) {
      iter = cookies_.erase(iter);
      erased = true;
    } else {
      iter++;
    }
  }

  return erased;
}

}  // namespace gdt
