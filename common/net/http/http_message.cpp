// Copyright 2011, Tencent Inc.
// Author: Hangjun Ye <hansye@tencent.com>
// Xiaokang Liu <hsiaokangliu@tencent.com>

#include "common/net/http/http_message.h"

#include <algorithm>
#include <string>
#include <vector>

#include "common/base/string/algorithm.h"
#include "common/base/string/concat.h"
#include "common/base/string/string_number.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/snappy/snappy.h"
#include "thirdparty/zlib/zlib.h"

namespace gdt {

static const struct {
  int version_number;
  const char* version_string;
} kHttpVersions[] = {
  { HttpMessage::VERSION_1_1, "HTTP/1.1" },
  { HttpMessage::VERSION_1_0, "HTTP/1.0" },
  { HttpMessage::VERSION_0_9, "HTTP/0.9" },
  { HttpMessage::VERSION_UNKNOWN, NULL },
};

void HttpMessage::Reset() {
  m_http_version = VERSION_1_1;
  m_headers.Clear();
  m_body.clear();
}

void HttpMessage::AppendHeadersToString(std::string* result) const {
  AppendStartLineToString(result);
  result->append("\r\n");
  m_headers.AppendToString(result);
  result->append("\r\n");
}

void HttpMessage::HeadersToString(std::string* result) const {
  result->clear();
  AppendHeadersToString(result);
}

std::string HttpMessage::HeadersToString() const {
  std::string result;
  AppendHeadersToString(&result);
  return result;
}

void HttpMessage::AppendToString(std::string* result) const {
  AppendHeadersToString(result);
  result->append(m_body);
}

void HttpMessage::ToString(std::string* result) const {
  result->clear();
  AppendToString(result);
}

std::string HttpMessage::ToString() const {
  std::string result;
  AppendToString(&result);
  return result;
}

// Get a header value. return false if it does not exist.
// the header name is not case sensitive.
bool HttpMessage::GetHeader(const StringPiece& header_name,
                            std::string** header_value) {
  return m_headers.Get(header_name, header_value);
}

bool HttpMessage::GetHeader(const StringPiece& header_name,
                            const std::string** header_value) const {
  return m_headers.Get(header_name, header_value);
}

bool HttpMessage::GetHeader(
  const StringPiece& header_name,
  std::string* value) const {
  const std::string* pvalue;
  if (GetHeader(header_name, &pvalue)) {
    *value = *pvalue;
    return true;
  }
  return false;
}

std::string HttpMessage::GetHeader(const StringPiece& header_name) const {
  std::string header_value;
  GetHeader(header_name, &header_value);
  return header_value;
}

// Used when a http header appears multiple times.
// return false if it doesn't exist.
bool HttpMessage::GetHeaders(const StringPiece& header_name,
                             std::vector<std::string>* header_values) const {
  return m_headers.Get(header_name, header_values);
}

// Set a header field. if it exists, overwrite the header value.
void HttpMessage::SetHeader(const StringPiece& header_name,
                            const StringPiece& header_value) {
  m_headers.Set(header_name, header_value);
}

void HttpMessage::SetHeaders(const HttpHeaders& headers) {
  m_headers = headers;
}

// Add a header field, just append, no overwrite.
void HttpMessage::AddHeader(const StringPiece& header_name,
                            const StringPiece& header_value) {
  m_headers.Add(header_name, header_value);
}

void HttpMessage::AddHeaders(const HttpHeaders& headers) {
  m_headers.Add(headers);
}

bool HttpMessage::RemoveHeader(const StringPiece& header_name) {
  return m_headers.Remove(header_name);
}

bool HttpMessage::HasHeader(const StringPiece& header_name) const {
  return m_headers.Has(header_name);
}

bool HttpMessage::CopyHeader(const StringPiece& header_name,
                             const HttpMessage& other) {
  std::string value;
  if (other.GetHeader(header_name, &value)) {
    SetHeader(header_name, value);
    return true;
  }
  return false;
}

const char* HttpMessage::GetVersionString(int version) {
  for (int i = 0; ; ++i) {
    if (kHttpVersions[i].version_number == VERSION_UNKNOWN) {
      return NULL;
    }
    if (version == kHttpVersions[i].version_number) {
      return kHttpVersions[i].version_string;
    }
  }
}

int HttpMessage::GetVersionNumber(const StringPiece& http_version) {
  if (!http_version.starts_with("HTTP/"))
    return HttpMessage::VERSION_UNKNOWN;
  static const size_t kPrefixLength = 5;  // "HTTP/"
  StringPiece version = http_version.substr(kPrefixLength);
  for (int i = 0; ; ++i) {
    if (kHttpVersions[i].version_string == NULL) {
      return HttpMessage::VERSION_UNKNOWN;
    }
    if (version == kHttpVersions[i].version_string + kPrefixLength) {
      return kHttpVersions[i].version_number;
    }
  }
}

// class HttpMessage
bool HttpMessage::ParseHeaders(const StringPiece& data,
                               HttpMessage::ErrorType* error) {
  HttpMessage::ErrorType error_placeholder;
  if (error == NULL)
    error = &error_placeholder;

  StringPiece::size_type pos = data.find_first_of('\n');
  if (pos == StringPiece::npos) {
    pos = data.size();
  }
  StringPiece first_line = data.substr(0, pos);
  RemoveLineEnding(&first_line);

  if (first_line.empty()) {
    *error = HttpMessage::ERROR_NO_START_LINE;
    return false;
  }

  if (!ParseStartLine(first_line, error))
    return false;

  int error_code = 0;
  bool result = m_headers.Parse(data.substr(pos + 1), &error_code);
  *error = static_cast<HttpMessage::ErrorType>(error_code);
  return result;
}

int HttpMessage::GetContentLength() {
  std::string content_length;
  if (!GetHeader("Content-Length", &content_length)) {
    return -1;
  }
  int length = 0;
  bool ret = StringToNumeric(content_length, &length);
  return (ret && length >= 0) ? length : -1;
}

void HttpMessage::UpdateContentLength() {
  std::string* value;
  if (GetHeader("Content-Length", &value)) {
    *value = ConvertToString(m_body.size());
  }
}

bool HttpMessage::IsKeepAlive() const {
  const std::string* alive;
  if (!GetHeader("Connection", &alive)) {
    if (m_http_version < VERSION_1_1) {
      return false;
    }
    return true;
  }
  return strcasecmp(alive->c_str(), "keep-alive") == 0;
}

const char kContentEncoding[] = "Content-Encoding";
const char kGzip[] = "gzip";
const char kDeflate[] = "deflate";
const char kSnappy[] = "snappy";
const char kIdentity[] = "identity";

static bool SnappyCompress(std::string* data);
static bool SnappyDecompress(std::string* data);

bool HttpMessage::Compress(const std::string& accept_encoding) {
  DCHECK(!HasHeader(kContentEncoding));
  std::vector<StringPiece> encodings;
  SplitString(accept_encoding, ",", &encodings);
  for (size_t i = 0; i < encodings.size(); ++i) {
    StringPiece encoding = encodings[i];
    bool result = true;
    if (encoding == kGzip) {
      result = GzipCompress(&m_body);
    } else if (encoding == kDeflate) {
      result = DeflateCompress(&m_body);
    } else if (encoding == kSnappy) {
      result = SnappyCompress(&m_body);
    } else if (encoding == kIdentity) {
      return true;
    } else {
      continue;
    }
    if (result) {
      SetHeader(kContentEncoding, encoding);
      UpdateContentLength();
      return true;
    }
    return false;
  }
  return false;
}

bool HttpMessage::Decompress() {
  std::string* content_encoding;
  if (GetHeader(kContentEncoding, &content_encoding)) {
    bool result = true;
    if (*content_encoding == kGzip) {
      result = GzipDecompress(&m_body);
    } else if (*content_encoding == kDeflate) {
      result = DeflateDecompress(&m_body);
    } else if (*content_encoding == kSnappy) {
      result = SnappyDecompress(&m_body);
    } else if (*content_encoding == kIdentity) {
      return true;
    } else {
      return false;
    }
    if (result) {
      // Keep the original Content-Encoding and Content-Length
      return true;
    }
    return false;
  }
  return true;
}

// Limit the max decompressed size to resist possible attack.
const size_t kMaxDecompressedSize = 32 * 1024 * 1024;

// Calculate data compression rate according to already processed data
// adaptively.
static double CompressionRate(const z_stream* stream, double default_value) {
  if (stream->total_in > 0)
    return static_cast<double>(stream->total_out) / stream->total_in + 0.5;
  return default_value;
}

static bool IsOverlapped(const void* src, const std::string* result) {
  const char* p = static_cast<const char*>(src);
  const char* begin = result->data();
  const char* end = begin + result->size();
  return p > begin && p < end;
}

static int DeflateData(z_stream* stream, const void *src, size_t src_length,
                       std::string* result) {
  DCHECK(!IsOverlapped(src, result));

  stream->next_in = reinterpret_cast<Bytef*>(const_cast<void*>(src));
  stream->avail_in = src_length;

  result->clear();

  for (;;) {
    double rate = CompressionRate(stream, 0.5);
    result->resize(result->size() + static_cast<size_t>(stream->avail_in * rate));
    stream->next_out = reinterpret_cast<Bytef*>(&(*result)[stream->total_out]);
    stream->avail_out = result->size() - stream->total_out;

    int code = deflate(stream, stream->avail_in ? Z_NO_FLUSH : Z_FINISH);

    if (code != Z_OK) {
      if (code == Z_STREAM_END)
        break;

      if (code == Z_BUF_ERROR) {
        // For finish flush
        result->resize(result->size() + 32);
        continue;
      }

      if (code < 0)
        return code;
    }
  }

  result->resize(stream->total_out);

  return Z_OK;
}

static int InflateData(z_stream* stream, const void *src, size_t src_length,
                       std::string* result) {
  DCHECK(!IsOverlapped(src, result));

  stream->next_in = reinterpret_cast<Bytef*>(const_cast<void*>(src));
  stream->avail_in = src_length;

  result->clear();

  for (;;) {
    double rate = CompressionRate(stream, 5);
    size_t new_size = std::min(kMaxDecompressedSize,
                               result->size() +
                               static_cast<size_t>(stream->avail_in * rate));
    if (new_size == result->size())
      return Z_MEM_ERROR;

    result->resize(std::max(kMaxDecompressedSize, new_size));
    stream->next_out = reinterpret_cast<Bytef*>(&(*result)[stream->total_out]);
    stream->avail_out = result->size() - stream->total_out;

    int code = inflate(stream, Z_NO_FLUSH);

    if (code == Z_STREAM_END)
      break;

    // if decompress complete, ignore error
    if (stream->avail_in == 0 && stream->avail_out > 0)
      break;

    if (code < 0 || code == Z_NEED_DICT)
      return code;
  }

  result->resize(stream->total_out);

  return Z_OK;
}

// See document of inflateInit2 in zlib.h
const int ZLIB_INIT_FLAG_GZIP = 16;

bool GzipCompress(StringPiece src, std::string* result) {
  z_stream stream = { 0 };

  int code = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                          MAX_WBITS + ZLIB_INIT_FLAG_GZIP, 8,
                          Z_DEFAULT_STRATEGY);
  if (code == Z_OK) {
    code = DeflateData(&stream, src.data(), src.length(), result);
    int end_code = deflateEnd(&stream);
    if (code == Z_OK)
      code = end_code;
  }

  return code == Z_OK;
}

bool GzipDecompress(StringPiece src, std::string* result) {
  z_stream stream = { 0 };

  int code = inflateInit2(&stream, MAX_WBITS + ZLIB_INIT_FLAG_GZIP);
  if (code == Z_OK) {
    code = InflateData(&stream, src.data(), src.length(), result);
    int end_code = inflateEnd(&stream);
    if (code == Z_OK)
      code = end_code;
  }

  return code == Z_OK;
}

bool DeflateCompress(StringPiece src, std::string* result) {
  z_stream stream = { 0 };

  int code = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
  if (code == Z_OK) {
    code = DeflateData(&stream, src.data(), src.length(), result);
    int end_code = deflateEnd(&stream);
    if (code == Z_OK)
      code = end_code;
  }

  return code == Z_OK;
}

bool DeflateDecompress(StringPiece src, std::string* result) {
  z_stream stream = { 0 };
  int code = inflateInit(&stream);
  if (code == Z_OK) {
    // See mozilla\netwerk\streamconv\converters\nsHTTPCompressConv.cpp
    // some servers (notably Apache with mod_deflate) don't generate zlib
    // headers, insert a dummy header and try again.
    static const unsigned char header[2] = { 0x78, 0x9C };

    if (!src.starts_with(StringPiece(header, sizeof(header)))) {
      stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(header));
      stream.avail_in = sizeof(header);

      unsigned char dummy_output[1];
      stream.next_out = dummy_output;
      stream.avail_out = 0;

      code = inflate(&stream, Z_NO_FLUSH);
      if (code == Z_OK)
        code = InflateData(&stream, src.data(), src.length(), result);
    } else {
      code = InflateData(&stream, src.data(), src.length(), result);
    }

    int end_code = inflateEnd(&stream);
    if (code == Z_OK)
      code = end_code;
  }

  return code == Z_OK;
}

#define DEFINE_INPLACE_COMPRESSION(method) \
bool method(std::string* data) { \
  std::string result; \
  if (!method(*data, &result)) { \
    return false; \
  } \
  data->swap(result); \
  return true; \
}

DEFINE_INPLACE_COMPRESSION(GzipCompress)
DEFINE_INPLACE_COMPRESSION(GzipDecompress)
DEFINE_INPLACE_COMPRESSION(DeflateCompress)
DEFINE_INPLACE_COMPRESSION(DeflateDecompress)

static bool SnappyCompress(std::string* data) {
  std::string result;
  if (!snappy::Compress(data->data(), data->size(), &result)) {
    return false;
  }
  data->swap(result);
  return true;
}

static bool SnappyDecompress(std::string* data) {
  size_t length;
  if (!snappy::GetUncompressedLength(data->data(), data->size(), &length))
    return false;
  if (length > kMaxDecompressedSize)
    return false;
  std::string result;
  if (!snappy::Uncompress(data->data(), data->size(), &result))
    return false;
  data->swap(result);
  return true;
}

}  // namespace gdt
