// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: DongPing HUANG <dphuang@tencent.com>
// Created: 11/05/11
// Description:

#include "common/net/http/http_downloader.h"
#include <string>
#include "common/base/string/concat.h"
#include "common/net/http/server/http_server.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

class HttpDownloaderTest : public testing::Test {
 protected:
  void SetUp() {
    m_server.RegisterStaticResource("/hello.txt", "hello");
    m_server.RegisterHttpHandler(
        "/ContentType",
        NewPermanentCallback(this,
                             &HttpDownloaderTest::HandleContentTypeRequest));
    m_server.RegisterHttpHandler(
        "/ChunkType",
        NewPermanentCallback(this,
                             &HttpDownloaderTest::HandleChunkTypeRequest));
    m_server.RegisterHttpHandler(
        "/Post",
        NewPermanentCallback(this, &HttpDownloaderTest::HandlePost));
    m_server.RegisterHttpHandler(
        "/Put",
        NewPermanentCallback(this, &HttpDownloaderTest::HandlePut));
    m_server.RegisterHttpHandler(
        "/Delete",
        NewPermanentCallback(this, &HttpDownloaderTest::HandleDelete));
    SocketAddressStorage server_address;
    m_server.Listen("127.0.0.1", 0, &server_address);
    m_server.Start();
    m_server_address = StringConcat("http://", server_address.ToString());
  }
  std::string m_server_address;

 private:
  void HandleContentTypeRequest(const HttpRequest* request,
                                HttpResponse* response) {
    std::string content_type;
    if (request->GetHeader("Content-Type", &content_type)) {
      response->SetHeader("Content-Type", content_type);
    } else {
      response->SetHeader("Content-Type", "text/plain");
    }
    response->set_body("hello, world!");
  }

  void HandleChunkTypeRequest(const HttpRequest* request,
                              HttpResponse* response) {
    response->SetHeader("Content-Type", "text/plain");
    response->SetHeader("Transfer-Encoding", "chunked");
    response->set_body("6\r\nhello,\r\n5\r\nworld\r\n0\r\n\r\n");
  }

  void HandlePost(const HttpRequest* request,
                  HttpResponse* response) {
    if (request->method() != HttpRequest::METHOD_POST)
      response->set_status(HttpResponse::Status_MethodNotAllowed);
    response->SetHeader("Content-Type", "text/plain");
    response->set_body(request->body());
  }

  void HandlePut(const HttpRequest* request, HttpResponse* response) {
    if (request->method() != HttpRequest::METHOD_PUT)
      response->set_status(HttpResponse::Status_MethodNotAllowed);
    response->SetHeader("Content-Type", "text/plain");
    response->set_body(request->body());
  }

  void HandleDelete(const HttpRequest* request, HttpResponse* response) {
    if (request->method() != HttpRequest::METHOD_DELETE)
      response->set_status(HttpResponse::Status_MethodNotAllowed);
    response->SetHeader("Content-Type", "text/plain");
    response->set_body("success");
  }

 private:
  HttpServer m_server;
};

TEST_F(HttpDownloaderTest, GetWithBadURI) {
  HttpDownloader downloader;
  HttpResponse response;
  HttpDownloader::ErrorType error;
  std::string bad_uri = "http://-www.qq.com";
  EXPECT_FALSE(downloader.Get(bad_uri, &response, &error));
  EXPECT_EQ(HttpDownloader::ERROR_INVALID_URI_ADDRESS, error);
}

TEST_F(HttpDownloaderTest, GetWithBadProxy) {
  HttpDownloader downloader;
  HttpResponse response;
  HttpDownloader::ErrorType error;
  downloader.SetProxy("http://-proxy.tencent.com:8080");
  EXPECT_FALSE(downloader.Get(m_server_address + "/hello.txt", &response,
                              &error));
  EXPECT_EQ(HttpDownloader::ERROR_INVALID_PROXY_ADDRESS, error);
}

TEST_F(HttpDownloaderTest, GetWithoutProxy) {
  HttpDownloader downloader;
  HttpResponse response;
  HttpDownloader::Options options;
  options.Headers().Add("Content-Type", "text/plain");
  downloader.SetUserAgent("TestDownloader");
  HttpDownloader::ErrorType error;
  EXPECT_TRUE(downloader.Get(m_server_address + "/hello.txt", &response,
                             &error));
  EXPECT_EQ(HttpDownloader::SUCCESS, error);
  EXPECT_EQ(HttpResponse::Status_OK, response.status());
  EXPECT_EQ("hello", response.body());

  EXPECT_FALSE(downloader.Get(m_server_address + "/world", &response, &error));
  EXPECT_EQ(HttpDownloader::ERROR_HTTP_STATUS_CODE, error);
  EXPECT_EQ(HttpResponse::Status_NotFound, response.status());

  EXPECT_FALSE(downloader.Get("http://127.0.0.1:8/world", &response, &error));
  EXPECT_EQ(HttpDownloader::ERROR_FAIL_TO_CONNECT_SERVER, error);
  EXPECT_STREQ("Failed to connect to server",
               downloader.GetErrorMessage(error));

  EXPECT_FALSE(downloader.Get("http://non-exist-domaon.test/world", &response,
                              &error));
  EXPECT_EQ(HttpDownloader::ERROR_FAIL_TO_RESOLVE_ADDRESS, error);
  EXPECT_STREQ("Failed to resolve address", downloader.GetErrorMessage(error));

  // not supported scheme
  EXPECT_FALSE(downloader.Get("ftp://127.0.0.1/hello.txt", &response, &error));

  // Receive with small response length
  HttpDownloader downloader2;
  options.SetMaxResponseLength(3);
  EXPECT_FALSE(downloader2.Get(m_server_address + "/hello.txt", options,
                               &response, &error));
  // room is not enough to store a complete http response header
  EXPECT_EQ(HttpDownloader::ERROR_INVALID_RESPONSE_HEADER, error);
}

TEST_F(HttpDownloaderTest, GetWithContentType) {
  HttpDownloader downloader;
  HttpResponse response;
  HttpDownloader::Options options;
  options.Headers().Add("Content-Type", "application/octet-stream");
  HttpDownloader::ErrorType error;
  EXPECT_TRUE(downloader.Get(m_server_address + "/ContentType",
                             options,
                             &response,
                             &error));
  EXPECT_EQ(HttpDownloader::SUCCESS, error);
  EXPECT_EQ(HttpResponse::Status_OK, response.status());
  std::string content_type;
  EXPECT_TRUE(response.GetHeader("Content-Type", &content_type));
  EXPECT_EQ("application/octet-stream", content_type);
}

TEST_F(HttpDownloaderTest, GetWithChunkResponse) {
  HttpDownloader downloader;
  HttpResponse response;
  HttpDownloader::Options options;
  HttpDownloader::ErrorType error;
  EXPECT_TRUE(downloader.Get(m_server_address + "/ChunkType",
                             options,
                             &response,
                             &error));
  EXPECT_EQ(HttpDownloader::SUCCESS, error);
  EXPECT_EQ(HttpResponse::Status_OK, response.status());
  EXPECT_EQ("hello,world", response.body());
}

TEST_F(HttpDownloaderTest, Post) {
  HttpDownloader downloader;
  HttpResponse response;
  HttpDownloader::Options options;
  HttpDownloader::ErrorType error;
  std::string data = "Post Content";
  EXPECT_TRUE(downloader.Post(m_server_address + "/Post",
                              data,
                              options,
                              &response,
                              &error));
  EXPECT_EQ(HttpDownloader::SUCCESS, error);
  EXPECT_EQ(HttpResponse::Status_OK, response.status());
  EXPECT_EQ(data, response.body());
}

TEST_F(HttpDownloaderTest, Put) {
  HttpDownloader downloader;
  HttpResponse response;
  HttpDownloader::Options options;
  HttpDownloader::ErrorType error;
  std::string data = "Put Content";
  EXPECT_TRUE(downloader.Put(m_server_address + "/Put",
                             data,
                             options,
                             &response,
                             &error));
  EXPECT_EQ(HttpDownloader::SUCCESS, error);
  EXPECT_EQ(HttpResponse::Status_OK, response.status());
  EXPECT_EQ(data, response.body());
}

TEST_F(HttpDownloaderTest, Delete) {
  HttpDownloader downloader;
  HttpResponse response;
  HttpDownloader::Options options;
  HttpDownloader::ErrorType error;
  EXPECT_TRUE(downloader.Delete(m_server_address + "/Delete",
                                options,
                                &response,
                                &error));
  EXPECT_EQ(HttpDownloader::SUCCESS, error);
  EXPECT_EQ(HttpResponse::Status_OK, response.status());
  EXPECT_EQ("success", response.body());
}

TEST_F(HttpDownloaderTest, Encoding) {
  HttpDownloader downloader;
  HttpDownloader::Options options;
  options.SetEncoding("GB2312");
  EXPECT_EQ("GB2312", options.Encoding());
}

bool TestDownloadUrlWithProxy(const std::string& url) {
  HttpDownloader downloader;
  downloader.SetProxy("http://proxy.tencent.com:8080");

  HttpResponse response;
  HttpDownloader::ErrorType error;
  if (!downloader.Get(url, &response, &error)) {
    LOG(ERROR)
        << "Fail to get " << url
        << "Error Message: " << HttpDownloader::GetErrorMessage(error);
    return false;
  }

  LOG(INFO) << "Succees to get " << url
            << "Response Header is: " << response.HeadersToString()
            << "Response is: " << response.body();
  return true;
}

}  // namespace gdt

