// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_ENCODING_BASE64_H
#define COMMON_ENCODING_BASE64_H
#pragma once

#include <string>
#include "common/base/string/string_piece.h"

// namespace common {

class Base64 {
 public:
  // Encodes the input string in base64.  Returns true if successful and false
  // otherwise.  The output string is only modified if successful.
  static bool Encode(const StringPiece& input, std::string* output);

  // Same as above, but use urlsafe character set.
  // See rfc4648: http://tools.ietf.org/html/rfc4648
  static bool RfcWebSafeEncode(const StringPiece& input, std::string* output);

  // Same as RfcWebSafeEncode, but skip the padding "=" character.
  // See rfc4648: http://tools.ietf.org/html/rfc4648#section-5
  static bool PaddingSkippedRfcWebSafeEncode(const StringPiece& input,
                                             std::string* output);

  // Decodes the base64 input string.  Returns true if successful and false
  // otherwise.  The output string is only modified if successful.
  static bool Decode(const StringPiece& input, std::string* output);

  // Same as above, but decode the result of RfcWebSafeEncode.
  // See rfc4648: http://tools.ietf.org/html/rfc4648
  static bool RfcWebSafeDecode(const StringPiece& input, std::string* output);

  // Same as RfcWebSafeDecode, but skip the padding "=" character.
  // See rfc4648: http://tools.ietf.org/html/rfc4648#section-5
  static bool PaddingSkippedRfcWebSafeDecode(const StringPiece& input,
                                             std::string* output);
};

// } // namespace common

#endif //  COMMON_ENCODING_BASE64_H

