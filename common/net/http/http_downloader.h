// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: DongPing HUANG <dphuang@tencent.com>
// Created: 11/05/11
// Description:

#ifndef COMMON_NET_HTTP_HTTP_DOWNLOADER_H_
#define COMMON_NET_HTTP_HTTP_DOWNLOADER_H_
#pragma once

#include <string>
#include <vector>

#include "common/net/http/http_request.h"
#include "common/net/http/http_response.h"
#include "common/system/net/socket_address.h"

namespace gdt {

// Helper class to download a page. Support GET/POST methods.
// Now only the easiest case is supported. In the future, we need to support
// some more complicated cases, for example, forward, encoding, response in
// stream mode( which is neccessary to download huge file.), etc.
class HttpDownloader {
 public:
  enum ErrorType {
    SUCCESS = 0,
    ERROR_INVALID_URI_ADDRESS,
    ERROR_INVALID_PROXY_ADDRESS,
    ERROR_INVALID_RESPONSE_HEADER,
    ERROR_FAIL_TO_RESOLVE_ADDRESS,
    ERROR_FAIL_TO_SEND_REQUEST,
    ERROR_FAIL_TO_GET_RESPONSE,
    ERROR_FAIL_TO_CONNECT_SERVER,
    ERROR_FAIL_TO_READ_CHUNKSIZE,
    ERROR_PROTOCOL_NOT_SUPPORTED,
    ERROR_CONTENT_TYPE_NOT_SUPPORTED,
    ERROR_HTTP_STATUS_CODE,    // such as HTTP 404
    ERROR_TOO_MANY_REDIRECTS,  // TODO(phongchen): support redirection
  };

  // query error message from error code
  static const char* GetErrorMessage(ErrorType error);

  // Resolve domain address, output is a vector of SocketAddressInet4
  // domain example:
  //  www.qq.com
  //  192.168.1.1
  static bool ResolveAddress(const std::string& domain,
                             uint16_t port,
                             std::vector<SocketAddressInet4> *address,
                             ErrorType *error = NULL);

 public:
  class Options {
   public:
    Options() : m_encoding(""), m_max_response_length(0) {}
    Options& SetEncoding(const std::string& encoding);
    const std::string& Encoding() const;
    Options& SetAcceptLanguage(const std::string& languages);
    const std::string& AccpetLanguage() const;
    HttpHeaders& Headers();
    const HttpHeaders& Headers() const;
    Options& SetMaxResponseLength(size_t length);
    size_t MaxResponseLength() const;
    void Swap(Options& rhs);

   private:
    std::string m_encoding;
    HttpHeaders m_headers;
    size_t m_max_response_length;
  };

 public:
  HttpDownloader();

  ~HttpDownloader();

  HttpDownloader& SetProxy(const std::string& proxy);

  const std::string& Proxy() const;

  HttpDownloader& SetUserAgent(const std::string& user_agent);

  const std::string& UserAgent() const;

  size_t GetMaxResponseLength() const;

  // Download url with GET method, output stored into response object.
  bool Get(const std::string& url,
           HttpResponse* response,
           ErrorType* error = NULL);

  // Download url with GET method, output stored into response object.
  // Some options supported, for example, http socket proxy.
  bool Get(const std::string& url,
           const Options& options,
           HttpResponse* response,
           ErrorType* error = NULL);

  // Download url with POST method, output stored into response object.
  bool Post(const std::string& url,
            const std::string& data,
            HttpResponse* response,
            ErrorType* error = NULL);

  // Download url with POST method, output stored into response object.
  // Some options supported, for example, http socket proxy.
  bool Post(const std::string& url,
            const std::string& data,
            const Options& options,
            HttpResponse* response,
            ErrorType* error = NULL);

  // Download url with PUT method, output stored into response object.
  // Some options supported, for example, http socket proxy.
  bool Put(const std::string& url,
           const std::string& data,
           HttpResponse* response,
           ErrorType* error = NULL);

  // Download url with PUT method, output stored into response object.
  // Some options supported, for example, http socket proxy.
  bool Put(const std::string& url,
           const std::string& data,
           const Options& options,
           HttpResponse* response,
           ErrorType* error = NULL);

  // Download url with DELETE method, output stored into response object.
  // Some options supported, for example, http socket proxy.
  bool Delete(const std::string& url,
              HttpResponse* response,
              ErrorType* error = NULL);

  // Download url with DELETE method, output stored into response object.
  // Some options supported, for example, http socket proxy.
  bool Delete(const std::string& url,
              const Options& options,
              HttpResponse* response,
              ErrorType* error = NULL);

 private:
  bool Request(HttpRequest::MethodType method,
               const std::string& url,
               const std::string& data,
               const Options& options,
               HttpResponse *response,
               ErrorType *error);

 private:
  static const struct ErrorMessage {
    ErrorType err_code;
    const char* err_msg;
  } kErrorMessage[];

 private:
  std::string m_proxy;
  std::string m_user_agent;
};

}  // namespace gdt

#endif  // COMMON_NET_HTTP_HTTP_DOWNLOADER_H_
