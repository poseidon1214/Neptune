// Copyright (c) 2015 Tencent Inc.
// Author: Sun Naicai (naicaisun@tencent.com)

#include "common/net/http/http_cookies.h"

#include "common/net/http/http_request.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

TEST(HttpCookie, ParseCookie) {
  std::string cookie;
  std::list<gdt::HttpCookie> cookie_list;
  HttpCookie::ParseCookie(cookie, &cookie_list);
  EXPECT_TRUE(cookie_list.empty());

  cookie = " hello = world ";
  HttpCookie::ParseCookie(cookie, &cookie_list);
  EXPECT_EQ(1, cookie_list.size());
  EXPECT_EQ("hello", cookie_list.front().name());
  EXPECT_EQ("world", cookie_list.front().value());

  cookie_list.clear();
  cookie = " hello = world; agent=gdt ";
  HttpCookie::ParseCookie(cookie, &cookie_list);
  EXPECT_EQ(2, cookie_list.size());
  EXPECT_EQ("agent", cookie_list.back().name());
  EXPECT_EQ("gdt", cookie_list.back().value());
}

TEST(HttpCookie, ParseCookieFromRequest) {
  HttpRequest http_request;
  http_request.AddHeader("Cookie", "a=1; b=2");
  http_request.AddHeader("Cookie", "c=@VL3oPN5SR");
  std::list<gdt::HttpCookie> cookie_list;
  HttpCookie::ParseCookieFromRequest(http_request, &cookie_list);

  ASSERT_EQ(3, cookie_list.size());
  std::list<gdt::HttpCookie>::const_iterator iter_cookie = cookie_list.begin();
  EXPECT_EQ("a", iter_cookie->name());
  EXPECT_EQ("1", iter_cookie->value());

  iter_cookie++;
  EXPECT_EQ("b", iter_cookie->name());
  EXPECT_EQ("2", iter_cookie->value());

  iter_cookie++;
  EXPECT_EQ("c", iter_cookie->name());
  EXPECT_EQ("@VL3oPN5SR", iter_cookie->value());
}

TEST(HttpCookies, ParseFromCookie) {
  std::string cookie = " hello = world ; agent=gdt";
  HttpCookies http_cookies;
  http_cookies.ParseFromCookie(cookie);
  std::string hello_value, agent_value;
  http_cookies.GetValue("hello", &hello_value);
  http_cookies.GetValue("agent", &agent_value);
  EXPECT_EQ("world", hello_value);
  EXPECT_EQ("gdt", agent_value);
}

TEST(HttpCookies, ParseFromRequest) {
  HttpRequest http_request;
  http_request.AddHeader("Cookie", "a=1; b=2");
  http_request.AddHeader("Cookie", "c=@VL3oPN5SR");

  HttpCookies http_cookies;
  http_cookies.ParseFromRequest(http_request);

  ASSERT_FALSE(http_cookies.IsEmpty());
  std::string a, b, c, d;
  EXPECT_TRUE(http_cookies.GetValue("a", &a));
  EXPECT_EQ("1", a);
  EXPECT_TRUE(http_cookies.GetValue("b", &b));
  EXPECT_EQ("2", b);
  EXPECT_TRUE(http_cookies.GetValue("c", &c));
  EXPECT_EQ("@VL3oPN5SR", c);
  EXPECT_FALSE(http_cookies.GetValue("d", &d));
}

TEST(HttpCookies, AddSet) {
  HttpCookies http_cookies;
  http_cookies.Set("abc", "123");  // will call Add
  http_cookies.Set(HttpCookie("len", "12"));  // will call Add
  http_cookies.Add("hello", "world");
  http_cookies.Add(HttpCookie("key", "yes"));

  std::string abc, len, hello, key;
  http_cookies.GetValue("abc", &abc);
  http_cookies.GetValue("len", &len);
  http_cookies.GetValue("hello", &hello);
  http_cookies.GetValue("key", &key);
  EXPECT_EQ("123", abc);
  EXPECT_EQ("12", len);
  EXPECT_EQ("world", hello);
  EXPECT_EQ("yes", key);

  http_cookies.Set("hello", "gdt");
  http_cookies.GetValue("hello", &hello);
  EXPECT_EQ("gdt", hello);
}

TEST(HttpCookies, Remove) {
  HttpCookies http_cookies;
  http_cookies.Add("hello", "world");
  std::string hello;
  http_cookies.GetValue("hello", &hello);
  EXPECT_EQ("world", hello);

  EXPECT_TRUE(http_cookies.Remove("hello"));
  EXPECT_TRUE(http_cookies.IsEmpty());
}

}  // namespace gdt
