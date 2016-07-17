// Copyright (c) 2015, Tencent Inc.
// Author: Li Wenting <wentingli@tencent.com>

#include "common/encoding/html.h"

#include <stdint.h>
#include <cstring>
#include "common/base/string/string_number.h"

namespace gdt {

bool HtmlEncode(const StringPiece& in, std::string* out) {
  std::string output;
  output.reserve(in.size() * 2);
  for (int i = 0; i < in.size(); i++) {
    switch (in[i]) {
      case '\"':
        output.append("&quot;");  // &#34;
        break;
      case '&':
        output.append("&amp;");  // &#38;
        break;
      case '<':
        output.append("&lt;");  // &#60;
        break;
      case '>':
        output.append("&gt;");  // &#62;
        break;
      default:
        output.push_back(in[i]);
        break;
    }
  }
  *out = output;
  return true;
}

static bool HtmlDecodeEntityName(const StringPiece& entity, int* c) {
  if (entity == "&quot;") {
    *c = '\"';
  } else if (entity == "&amp;") {
    *c = '&';
  } else if (entity == "&lt;") {
    *c = '<';
  } else if (entity == "&gt;") {
    *c = '>';
  } else if (entity == "&nbsp;") {
    *c = ' ';
  } else {
    return false;
  }
  return true;
}

static bool HtmlDecodeEntityNumber(const StringPiece& entity, int* c) {
  // Entity Number: &#nnnn; or &#xhhhh;
  char buf[10] = {0};
  int length = entity.length();
  // Max entity number in HTML 4.0 is &#x2666;
  const int kEntityNumberMaxLength = 8;
  if (length > kEntityNumberMaxLength)
    return false;

  memcpy(buf, entity.data(), length);
  const char* p = buf;
  const char* p_end = p + length - 1;  // points to the final semicolon
  p += 2;
  int base = 10;
  if (*p == 'x' || *p == 'X') {
    p++;
    base = 16;
  }
  char* end = NULL;
  return ParseNumber(p, c, &end, base) && end == p_end;
}

static bool HtmlDecodeEntity(const StringPiece& entity, int* c) {
  // entity should be at least 4 characters
  if (entity.length() < 4)
    return false;

  if (entity[1] == '#') {
    return HtmlDecodeEntityNumber(entity, c) && *c < 127;
  } else {
    return HtmlDecodeEntityName(entity, c);
  }
}

static bool HtmlDecode(const StringPiece& in,
                       std::string* out,
                       bool ignore_error) {
  std::string output;
  output.reserve(in.size());
  for (int i = 0; i < in.size();) {
    if (in[i] == '&') {
      StringPiece::size_type pos = in.find(';', i + 1);
      if (pos == StringPiece::npos) {
        if (!ignore_error)
          return false;

        output.append(in.substr(i).as_string());
        break;
      }

      int c = 0;
      const StringPiece entity = in.substr(i, pos - i + 1);
      if (HtmlDecodeEntity(entity, &c)) {
        output.push_back(c);
      } else {
        if (!ignore_error)
          return false;

        entity.append_to_string(&output);
      }
      i = pos + 1;
    } else {
      output.push_back(in[i]);
      i++;
    }
  }
  *out = output;
  return true;
}

bool HtmlDecode(const StringPiece& in, std::string* out) {
  return HtmlDecode(in, out, false);
}

std::string HtmlDecodeIgnoreError(const StringPiece& in) {
  std::string output;
  HtmlDecode(in, &output, true);
  return output;
}

}  // namespace gdt
