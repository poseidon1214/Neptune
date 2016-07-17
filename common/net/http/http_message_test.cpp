// Copyright 2011, Tencent Inc.
// Xiaokang Liu <hsiaokangliu@tencent.com>

#include "common/net/http/http_message.h"
#include <algorithm>
#include <string>
#include "app/qzap/common/utility/file_utility.h"
#include "common/base/string/algorithm.h"
#include "common/net/http/http_request.h"
#include "common/net/http/http_response.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

TEST(HttpMessage, HasHeader) {
  HttpRequest req;
  EXPECT_FALSE(req.HasHeader("Test"));
  EXPECT_FALSE(req.RemoveHeader("Test"));
  req.AddHeader("Test", "1");
  EXPECT_TRUE(req.HasHeader("Test"));
  EXPECT_TRUE(req.RemoveHeader("Test"));
  EXPECT_FALSE(req.HasHeader("Test"));
}

TEST(HttpMessage, AddHeader) {
  HttpRequest req;
  EXPECT_FALSE(req.HasHeader("Test"));
  req.AddHeader("Test", "1");
  EXPECT_TRUE(req.HasHeader("Test"));
}

TEST(HttpMessage, RemoveHeader) {
  HttpRequest req;
  EXPECT_FALSE(req.HasHeader("Test"));
  EXPECT_FALSE(req.RemoveHeader("Test"));
  req.AddHeader("Test", "1");
  EXPECT_TRUE(req.HasHeader("Test"));
  EXPECT_TRUE(req.RemoveHeader("Test"));
  EXPECT_FALSE(req.HasHeader("Test"));
}

TEST(HttpMessage, CopyHeader) {
  HttpRequest org;
  org.AddHeader("Test", "1");
  HttpRequest message;
  EXPECT_TRUE(message.CopyHeader("Test", org));
  EXPECT_TRUE(message.HasHeader("Test"));
  EXPECT_FALSE(message.CopyHeader("None", org));
  EXPECT_FALSE(message.HasHeader("None"));
}

TEST(HttpMessage, ToString) {
  HttpRequest request;
  request.set_method(HttpRequest::METHOD_GET);
  request.SetHeader("Hello", "World");
  request.set_body("Kitty");
  std::string s;
  request.ToString(&s);
  EXPECT_EQ("GET / HTTP/1.1\r\nHello: World\r\n\r\nKitty", s);
}

TEST(HttpMessage, HeadersToString) {
  HttpRequest request;
  request.set_method(HttpRequest::METHOD_GET);
  request.SetHeader("Hello", "World");
  request.set_body("Kitty");
  std::string s;
  request.HeadersToString(&s);
  EXPECT_EQ("GET / HTTP/1.1\r\nHello: World\r\n\r\n", s);
}

TEST(HttpMessage, GetHeader) {
  HttpRequest request;
  request.SetHeader("Hello", "World");
  std::string s;
  EXPECT_TRUE(request.GetHeader("Hello", &s));
  EXPECT_EQ("World", s);
  EXPECT_FALSE(request.GetHeader("Kitty", &s));
}

TEST(HttpMessage, SetHeaders) {
  HttpRequest request;
  HttpHeaders headers;
  headers.Add("Hello", "World");
  headers.Add("Accept", "text/html");
  request.SetHeaders(headers);
  std::string s;
  EXPECT_TRUE(request.GetHeader("Hello", &s));
  EXPECT_EQ("World", s);
  EXPECT_TRUE(request.GetHeader("Accept", &s));
  EXPECT_EQ("text/html", s);
}

TEST(HttpMessage, GetMethodName) {
  EXPECT_STREQ("HEAD", HttpRequest::GetMethodName(HttpRequest::METHOD_HEAD));
  EXPECT_STREQ("GET", HttpRequest::GetMethodName(HttpRequest::METHOD_GET));
  EXPECT_STREQ("POST", HttpRequest::GetMethodName(HttpRequest::METHOD_POST));
  EXPECT_STREQ("DELETE",
               HttpRequest::GetMethodName(HttpRequest::METHOD_DELETE));
  EXPECT_STREQ("OPTIONS", HttpRequest::GetMethodName(HttpRequest::METHOD_OPTIONS));
  EXPECT_STREQ("TRACE", HttpRequest::GetMethodName(HttpRequest::METHOD_TRACE));
  EXPECT_STREQ("CONNECT", HttpRequest::GetMethodName(
                 HttpRequest::METHOD_CONNECT));
  EXPECT_TRUE(HttpRequest::GetMethodName(HttpRequest::METHOD_UNKNOWN) == NULL);
}

TEST(HttpMessage, GetMethodByName) {
  EXPECT_EQ(HttpRequest::METHOD_HEAD, HttpRequest::GetMethodByName("HEAD"));
  EXPECT_EQ(HttpRequest::METHOD_GET, HttpRequest::GetMethodByName("GET"));
  EXPECT_EQ(HttpRequest::METHOD_POST, HttpRequest::GetMethodByName("POST"));
  EXPECT_EQ(HttpRequest::METHOD_DELETE, HttpRequest::GetMethodByName("DELETE"));
  EXPECT_EQ(HttpRequest::METHOD_OPTIONS, HttpRequest::GetMethodByName("OPTIONS"));
  EXPECT_EQ(HttpRequest::METHOD_TRACE, HttpRequest::GetMethodByName("TRACE"));
  EXPECT_EQ(HttpRequest::METHOD_CONNECT, HttpRequest::GetMethodByName("CONNECT"));
  EXPECT_EQ(HttpRequest::METHOD_UNKNOWN, HttpRequest::GetMethodByName("UNKNOWN"));
}

TEST(HttpMessage, UpdateContentLength) {
  HttpRequest message;
  message.set_body("test");
  message.UpdateContentLength();
  EXPECT_FALSE(message.HasHeader("Content-Length"));
  message.SetHeader("Content-Length", "4");
  message.set_body("test1");
  message.UpdateContentLength();
  EXPECT_EQ("5", message.GetHeader("Content-Length"));
}

static void TestCompression(const std::string& accept_encoding,
                            const std::string& expected_encoding) {
  HttpResponse message;
  const std::string plain = "hello, world!";
  message.set_body(plain);
  ASSERT_TRUE(message.Compress(accept_encoding));
  ASSERT_EQ(expected_encoding, message.GetHeader("Content-Encoding"));
  if (expected_encoding != "")
    ASSERT_NE(plain, message.body()) << accept_encoding;
  ASSERT_TRUE(message.Decompress());
  ASSERT_EQ(plain, message.body());
  if (expected_encoding != "")
    ASSERT_TRUE(message.HasHeader("Content-Encoding"));
}

TEST(HttpMessage, Compression) {
  TestCompression("gzip", "gzip");
  TestCompression("deflate", "deflate");
  TestCompression("snappy", "snappy");
  TestCompression("identity", "");
  TestCompression("gzip, deflate", "gzip");
  TestCompression("deflate, gzip", "deflate");
}

TEST(Compression, Gzip) {
  const std::string origin = "hello, world!";
  std::string compressed;
  ASSERT_TRUE(GzipCompress(origin, &compressed));
  EXPECT_TRUE(StringStartsWith(compressed, "\x1F\x8B"));  // GZIP Header
  std::string decompressed;
  ASSERT_TRUE(GzipDecompress(compressed, &decompressed));
  EXPECT_EQ(origin, decompressed);
}

TEST(Compression, Deflate) {
  const std::string origin = "hello, world!";
  std::string compressed;
  ASSERT_TRUE(DeflateCompress(origin, &compressed));
  EXPECT_TRUE(StringStartsWith(compressed, "\x78\x9C"));  // ZLIB Header
  std::string decompressed;
  ASSERT_TRUE(DeflateDecompress(compressed, &decompressed));
  EXPECT_EQ(origin, decompressed);
}

TEST(Compression, GzipFile) {
  std::string content;
  ASSERT_TRUE(ReadFileToString("testdata.gz", &content));
  std::string plain;
  ASSERT_TRUE(GzipDecompress(content, &plain));
  ASSERT_EQ("hello, world!", plain);
}

TEST(Compression, MaxSize) {
  std::string origin(32 * 1024 * 1024, 'A');
  std::string compressed;
  ASSERT_TRUE(GzipCompress(origin, &compressed));
  std::string plain;
  ASSERT_TRUE(GzipDecompress(compressed, &plain));
  origin.push_back('B');
  ASSERT_TRUE(GzipCompress(origin, &compressed));
  ASSERT_FALSE(GzipDecompress(compressed, &plain));
}

}  // namespace gdt
