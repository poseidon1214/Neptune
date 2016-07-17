// Copyright (c) 2011, Tencent.com
// All rights reserved.

/// @file query_params_test.cc
/// @brief cgi param test
/// @date  03/30/2011 11:05:46 PM
/// @author CHEN Feng <phongchen@tencent.com>

#include "common/net/uri/query_params.h"
#include "common/net/http/http_request.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

TEST(QueryParams, Parse) {
  QueryParams params;
  ASSERT_TRUE(params.Parse("a=1&b=2&c=%FF%FE&d"));
  ASSERT_EQ(4U, params.Count());
  ASSERT_EQ("a", params.Get(0).name);
  ASSERT_EQ("1", params.Get(0).value);

  ASSERT_EQ("b", params.Get(1).name);
  ASSERT_EQ("2", params.Get(1).value);

  ASSERT_EQ("c", params.Get(2).name);
  ASSERT_EQ("\xFF\xFE", params.Get(2).value);

  ASSERT_EQ("d", params.Get(3).name);
  ASSERT_EQ("", params.Get(3).value);

  // 测试Parse失败的情况
  // %后跟的数据长度不够
  ASSERT_FALSE(params.Parse("a=1&&b=%a"));
  // %后跟的数据不是hex digit
  ASSERT_FALSE(params.Parse("a=1&&b=%km"));
}

TEST(QueryParams, ParseUrl) {
  std::string url = "/json/task.json?task_type=kReduceTask&"
                    "task_status=kRunningTask&offset=0&length=10";
  QueryParams params;
  ASSERT_TRUE(params.IsEmpty());
  ASSERT_TRUE(params.ParseFromUrl(url));
  ASSERT_FALSE(params.IsEmpty());
  std::string task_type = params.GetOrDefaultValue("task_type", "");
  std::string task_status = params.GetOrDefaultValue("task_status", "");
  EXPECT_EQ("kReduceTask", task_type);
  EXPECT_EQ("kRunningTask", task_status);

  std::string offset, length, content;
  int32_t length_value, content_value;
  EXPECT_TRUE(params.GetValue("offset", &offset));
  EXPECT_EQ("0", offset);
  EXPECT_TRUE(params.GetValue("length", &length));
  EXPECT_EQ("10", length);
  EXPECT_FALSE(params.GetValue("content", &content));
  EXPECT_TRUE(params.GetValue("length", &length_value));
  EXPECT_EQ(10, length_value);
  EXPECT_FALSE(params.GetValue("content", &content_value));
}

TEST(QueryParams, ParseHttpRequest) {
  HttpRequest request;
  request.set_method(HttpRequest::METHOD_POST);
  request.set_uri("/?fromuri1=1&fromuri2=2");
  request.set_body("frombody1=1&frombody2=2");
  request.AddHeader("Content-Type", "application/x-www-form-urlencoded");
  QueryParams params;
  ASSERT_TRUE(params.ParseFromRequest(&request));
  EXPECT_EQ(1, params.GetAsInt32("fromuri1"));
  EXPECT_EQ(2, params.GetAsInt32("fromuri2"));
  EXPECT_EQ(1, params.GetAsInt32("frombody1"));
  EXPECT_EQ(2, params.GetAsInt32("frombody2"));
}

TEST(QueryParams, ToString) {
  const std::string kParamString = "a=1&b=2&c=%FF%FE&d=";
  std::string empty_string;
  std::string not_empty_string = "testwrite";
  QueryParams params;
  ASSERT_TRUE(params.Parse(kParamString));
  ASSERT_EQ(kParamString, params.ToString());
  params.WriteToString(&empty_string);
  params.WriteToString(&not_empty_string);
  ASSERT_EQ(kParamString, empty_string);
  ASSERT_EQ(kParamString, not_empty_string);
}

TEST(QueryParams, AddSet) {
  QueryParams params;
  QueryParam param_to_add;
  param_to_add.name = "length";
  param_to_add.value = "10";
  params.Add(param_to_add);
  params.Add("offset", "20");
  EXPECT_EQ("length=10&offset=20", params.ToString());
  params.Set("offset", "30");
  EXPECT_EQ("length=10&offset=30", params.ToString());
  params.Set("non_exist", "true");
  EXPECT_EQ("length=10&offset=30&non_exist=true", params.ToString());
  EXPECT_TRUE(params.Remove("offset"));
  EXPECT_FALSE(params.Remove("offset"));
  EXPECT_EQ("length=10&non_exist=true", params.ToString());

  // Add multiple same name param
  params.Add("length", "20");
  EXPECT_EQ("length=10&non_exist=true&length=20", params.ToString());
  // Remove just erase the first appeared name
  EXPECT_TRUE(params.Remove("length"));
  EXPECT_EQ("non_exist=true&length=20", params.ToString());
}

TEST(QueryParams, Int) {
  QueryParams params;
  params.Add("a", 1);
  params.Add("b", 2);
  int32_t a;
  int64_t b;
  EXPECT_TRUE(params.GetValue("a", &a));
  EXPECT_EQ(1, a);
  EXPECT_TRUE(params.GetValue("b", &b));
  EXPECT_EQ(2, b);
  EXPECT_FALSE(params.GetValue("c", &a));
  EXPECT_EQ(1, params.GetAsInt32("a"));
  EXPECT_EQ(2, params.GetAsInt64("b", 0));
  EXPECT_EQ(3, params.GetAsInt32("c", 3));
  params.Add("c", 3);
  uint32_t c = 0;
  EXPECT_TRUE(params.GetValue("c", &c));
  EXPECT_EQ(3, c);
}

}  // namespace gdt
