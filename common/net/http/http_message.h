// Copyright 2011, Tencent Inc.
// Author: Hangjun Ye <hansye@tencent.com>
// Xiaokang Liu <hsiaokangliu@tencent.com>

#ifndef COMMON_NET_HTTP_HTTP_MESSAGE_H_
#define COMMON_NET_HTTP_HTTP_MESSAGE_H_

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include "common/base/string/string_piece.h"
#include "common/net/http/http_headers.h"

namespace gdt {

// Describes an http message, which is the base class for http request and
// response. It includes the start line, headers and body.
class HttpMessage {
 public:
  enum Version {
    VERSION_UNKNOWN = 0,
    VERSION_0_9 = 9,
    VERSION_1_0 = 10,
    VERSION_1_1 = 11,
  };

  enum ErrorType {
    SUCCESS = 0,
    ERROR_NO_START_LINE,
    ERROR_START_LINE_NOT_COMPLETE,
    ERROR_VERSION_UNSUPPORTED,
    ERROR_RESPONSE_STATUS_NOT_FOUND,
    ERROR_FIELD_NOT_COMPLETE,
    ERROR_METHOD_NOT_FOUND,
    ERROR_MESSAGE_NOT_COMPLETE,
  };

  HttpMessage() {
    m_http_version = VERSION_1_1;
  }
  virtual ~HttpMessage() {}
  virtual void Reset();

 public:
  // Parse http headers (including the start line) from data.
  // return: error code which is defined as ErrorType.
  virtual bool ParseHeaders(const StringPiece& data, ErrorType* error = NULL);

  std::string StartLine() const {
    std::string result;
    AppendStartLineToString(&result);
    return result;
  }

  int http_version() const {
    return m_http_version;
  }
  void set_http_version(int version) {
    m_http_version = version;
  }

  const std::string& body() const {
    return m_body;
  }
  std::string* mutable_body() {
    return &m_body;
  }

  void set_body(const StringPiece& body) {
    m_body.assign(body.data(), body.size());
  }

  // string of GNU libstdc++ use reference count to reduce copy
  // keep these overloadings to help it
  void set_body(const std::string& body) {
    m_body = body;
  }
  void set_body(const char* body) {
    m_body.assign(body);
  }

  // Return -1 if no Content-Length exist.
  int GetContentLength();

  // Update the Content-Length header according to the header size if it exist.
  void UpdateContentLength();

  bool IsKeepAlive() const;

  // Get the header value.
  const HttpHeaders& headers() const {
    return m_headers;
  }

  HttpHeaders& headers() {
    return m_headers;
  }

  // Return false if it doesn't exist.
  bool GetHeader(const StringPiece& header_name, std::string** value);
  bool GetHeader(const StringPiece& header_name,
                 const std::string** value) const;
  bool GetHeader(const StringPiece& header_name, std::string* value) const;
  std::string GetHeader(const StringPiece& header_name) const;

  // Used when a http header appears multiple times.
  // return false if it doesn't exist.
  bool GetHeaders(const StringPiece& header_name,
                  std::vector<std::string>* header_values) const;
  // Set a header field. if it exists, overwrite the header value.
  void SetHeader(const StringPiece& header_name,
                 const StringPiece& header_value);
  // Replace the header with those in parameters 'headers'
  void SetHeaders(const HttpHeaders& headers);
  // Add a header field, just append, no overwrite.
  void AddHeader(const StringPiece& header_name,
                 const StringPiece& header_value);
  // Insert the items from 'headers'
  void AddHeaders(const HttpHeaders& headers);
  // Remove an http header field.
  bool RemoveHeader(const StringPiece& header_name);

  // If has a header
  bool HasHeader(const StringPiece& header_name) const;

  // Copy a header from other with header_name
  bool CopyHeader(const StringPiece& header_name, const HttpMessage& other);

  // Convert start line and headers to string.
  void AppendHeadersToString(std::string* result) const;
  void HeadersToString(std::string* result) const;
  std::string HeadersToString() const;

  void AppendToString(std::string* result) const;
  void ToString(std::string* result) const;
  std::string ToString() const;

  // Compress the response body according to the accept_encoding.
  // Return true if success, and then set the corresponding Content-Encoding and
  // update the existed Content-Length header if the selected encoding is not
  // identity.
  // Otherwise, return false, and keep the message unchanged.
  bool Compress(const std::string& accept_encoding);

  // Decompress the message body according to the Content-Encoding header.
  // Return true if success or Content-Encoding header does not existed.
  // Otherwise, return false, and keep the message unchanged.
  bool Decompress();

 protected:
  static const char* GetVersionString(int version);
  static int  GetVersionNumber(const StringPiece& http_version);

  // append without ending "\r\n"
  virtual void AppendStartLineToString(std::string* result) const = 0;
  virtual bool ParseStartLine(const StringPiece& data,
                              HttpMessage::ErrorType* error) = 0;
  void Swap(HttpMessage* other) {
    using std::swap;
    swap(m_http_version, other->m_http_version);
    m_headers.Swap(&other->m_headers);
    swap(m_body, other->m_body);
  }

 private:
  int m_http_version;
  HttpHeaders m_headers;
  std::string m_body;
};

// For Content-Encoding: gzip
bool GzipCompress(StringPiece src, std::string* result);
bool GzipDecompress(StringPiece src, std::string* result);
bool GzipCompress(std::string* data);
bool GzipDecompress(std::string* data);

// For Content-Encoding: deflate
bool DeflateCompress(StringPiece src, std::string* result);
bool DeflateDecompress(StringPiece src, std::string* result);
bool DeflateCompress(std::string* data);
bool DeflateDecompress(std::string* data);

}  // namespace gdt

#endif  // COMMON_NET_HTTP_HTTP_MESSAGE_H_
