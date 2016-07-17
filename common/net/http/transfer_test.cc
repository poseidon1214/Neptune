// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/net/http/transfer.h"
#include <string>
#include "common/net/http/http_request.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

TEST(HttpTransfer, DetectHeaderSizeNotEnd) {
  EXPECT_EQ(0, DetectHttpHeaderSize("HTTP/1.1 200 OK"));
}

TEST(HttpTransfer, DetectHeaderSize) {
  HttpRequest req;
  req.set_method(HttpRequest::METHOD_GET);
  req.set_uri("/");
  std::string req_str = req.ToString();
  EXPECT_EQ(static_cast<int>(req_str.size()), DetectHttpHeaderSize(req_str));

  req.set_body("hello");
  std::string req_with_body_str = req.ToString();
  EXPECT_EQ(static_cast<int>(req_str.size()),
            DetectHttpHeaderSize(req_with_body_str));
}

TEST(HttpTransfer, DetectHeaderSizeTooLarge) {
  HttpRequest req;
  req.set_method(HttpRequest::METHOD_GET);
  req.set_uri("/");
  req.SetHeader("Large-Header", std::string(kMaxHttpHeadersSize, 'A'));
  std::string req_str = req.ToString();
  EXPECT_EQ(-1, DetectHttpHeaderSize(req_str));
}

TEST(HttpTransfer, DetectHeaderSizeTooLargeNotEnd) {
  std::string req_str(kMaxHttpHeadersSize + 1, 'A');
  EXPECT_EQ(-1, DetectHttpHeaderSize(req_str));
}

}  // namespace gdt
