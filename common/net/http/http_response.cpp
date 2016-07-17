// Copyright (c) 2012, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 03/05/12
// Description: HttpResponse class implementation

#include "common/net/http/http_response.h"
#include <string>
#include <vector>
#include "common/base/string/algorithm.h"
#include "common/base/string/concat.h"

namespace gdt {

static const struct {
  int status_code;
  const char* reason_phrase;
  const char* status_description;
} kResponseStatus[] = {
  { 100, "Continue", "Request received, please continue" },
  { 101, "Switching Protocols", "Switching to new protocol; obey Upgrade header" },
  { 200, "OK", "Request fulfilled, document follows" },
  { 201, "Created", "Document created, URL follows" },
  { 202, "Accepted", "Request accepted, processing continues off-line" },
  { 203, "Non-Authoritative Information", "Request fulfilled from cache" },
  { 204, "No Content", "Request fulfilled, nothing follows" },
  { 205, "Reset Content", "Clear input form for further input." },
  { 206, "Partial Content", "Partial content follows." },
  { 300, "Multiple Choices", "Object has several resources -- see URI list" },
  { 301, "Moved Permanently", "Object moved permanently -- see URI list" },
  { 302, "Found", "Object moved temporarily -- see URI list" },
  { 303, "See Other", "Object moved -- see Method and URL list" },
  { 304, "Not Modified", "Document has not changed since given time" },
  { 305, "Use Proxy", "You must use proxy specified in Location to access this resource." },
  { 307, "Temporary Redirect", "Object moved temporarily -- see URI list" },
  { 400, "Bad Request", "Bad request syntax or unsupported method" },
  { 401, "Unauthorized", "No permission -- see authorization schemes" },
  { 402, "Payment Required", "No payment -- see charging schemes" },
  { 403, "Forbidden", "Request forbidden -- authorization will not help" },
  { 404, "Not Found", "Nothing matches the given URI" },
  { 405, "Method Not Allowed", "Specified method is invalid for this resource." },
  { 406, "Not Acceptable", "URI not available in preferred format." },
  { 407, "Proxy Authentication Required", "You must authenticate with this proxy before proceeding." },
  { 408, "Request Timeout", "Request timed out; try again later." },
  { 409, "Conflict", "Request conflict." },
  { 410, "Gone", "URI no longer exists and has been permanently removed." },
  { 411, "Length Required", "Client must specify Content-Length." },
  { 412, "Precondition Failed", "Precondition in headers is false." },
  { 413, "Request Entity Too Large", "Entity is too large." },
  { 414, "Request-URI Too Long", "URI is too long." },
  { 415, "Unsupported Media Type", "Entity body in unsupported format." },
  { 416, "Requested Range Not Satisfiable", "Cannot satisfy request range." },
  { 417, "Expectation Failed", "Expect condition could not be satisfied." },
  { 500, "Internal Server Error", "Server got itself in trouble" },
  { 501, "Not Implemented", "Server does not support this operation" },
  { 502, "Bad Gateway", "Invalid responses from another server/proxy." },
  { 503, "Service Unavailable", "The server cannot process the request due to a high load" },
  { 504, "Gateway Timeout", "The gateway server did not receive a timely response" },
  { 505, "HTTP Version Not Supported", "Cannot fulfill request." },
  { -1, NULL, NULL },
};

// static
const char* HttpResponse::InternalStatusCodeToReasonPhrase(
  int status_code,
  const char* no_match) {
  for (int i = 0; ; ++i) {
    if (kResponseStatus[i].status_code == -1) {
      return no_match;
    }
    if (kResponseStatus[i].status_code == status_code) {
      return kResponseStatus[i].reason_phrase;
    }
  }
}

const char* HttpResponse::StatusCodeToDescription(int status_code) {
  for (int i = 0; ; ++i) {
    if (kResponseStatus[i].status_code == -1) {
      return "Unknown";
    }
    if (kResponseStatus[i].status_code == status_code) {
      return kResponseStatus[i].status_description;
    }
  }
}

bool HttpResponse::ParseStartLine(const StringPiece& data,
                                  HttpMessage::ErrorType* error) {
  ErrorType error_placeholder;
  if (error == NULL)
    error = &error_placeholder;

  std::vector<StringPiece> fields;
  SplitString(data, " ", &fields);
  if (fields.size() < 2) {
    *error = ERROR_START_LINE_NOT_COMPLETE;
    return false;
  }

  int http_version = GetVersionNumber(fields[0]);
  if (http_version == HttpMessage::VERSION_UNKNOWN) {
    *error = ERROR_VERSION_UNSUPPORTED;
    return false;
  }
  set_http_version(http_version);

  if (fields[1].size() > 3) {
    *error = ERROR_RESPONSE_STATUS_NOT_FOUND;
    return false;
  }
  char status_buffer[4];
  memcpy(status_buffer, fields[1].data(), fields[1].size());
  status_buffer[fields[1].size()] = '\0';
  m_status = atoi(status_buffer);
  if (m_status < 100 || m_status > 999) {
    *error = ERROR_RESPONSE_STATUS_NOT_FOUND;
    return false;
  }

  return true;
}

void HttpResponse::AppendStartLineToString(std::string* result) const {
  StringAppend(
    result,
    GetVersionString(http_version()),
    " ",
    m_status,
    " ",
    StatusCodeToReasonPhraseSafe(m_status));
}

void HttpResponse::FillWithHtmlPage(int code, const StringPiece& title,
                                    const StringPiece& body) {
  set_status(code);
  SetHeader("Content-Type", "text/html");
  std::string* http_body = mutable_body();
  http_body->assign("<html>\n<head>\n<title>");
  if (!title.empty())
    title.append_to_string(http_body);
  else
    StringAppend(http_body, "HTTP ", code, " ",
                 StatusCodeToReasonPhraseSafe(code));
  http_body->append("</title>\n</head>\n");
  http_body->append("<body>");
  if (!body.empty())
    body.append_to_string(http_body);
  else
    http_body->append(StatusCodeToDescription(code));
  http_body->append("</body>\n</html>\n");
}

void HttpResponse::Reset() {
  HttpMessage::Reset();
  m_status = 0;
}

void HttpResponse::AppendBodyChunk(StringPiece chunk) {
  char size[9];
  snprintf(size, sizeof(size), "%X", static_cast<unsigned>(chunk.size()));
  mutable_body()->append(size);
  mutable_body()->append("\r\n");
  chunk.append_to_string(mutable_body());
  mutable_body()->append("\r\n");
}

}  // namespace gdt

