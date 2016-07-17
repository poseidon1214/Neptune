// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/encoding/base64.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/concat.h"
#include "thirdparty/stringencoders/modp_b64.h"
#include "thirdparty/stringencoders/modp_b64r.h"
#include "thirdparty/stringencoders/modp_b64w.h"

// namespace common {

bool Base64::Encode(const StringPiece& input, std::string* output) {
  std::string temp;
  temp.resize(modp_b64_encode_len(input.size()));  // makes room for null byte

  // null terminates result since result is base64 text!
  int input_size = static_cast<int>(input.size());
  int output_size = modp_b64_encode(&(temp[0]), input.data(), input_size);
  if (output_size < 0)
    return false;

  temp.resize(output_size);  // strips off null byte
  output->swap(temp);
  return true;
}

bool Base64::RfcWebSafeEncode(const StringPiece& input, std::string* output) {
  std::string temp;
  temp.resize(modp_b64r_encode_len(input.size()));  // makes room for null byte

  // null terminates result since result is base64 text!
  int input_size = static_cast<int>(input.size());
  int output_size = modp_b64r_encode(&(temp[0]), input.data(), input_size);
  if (output_size < 0)
    return false;

  temp.resize(output_size);  // strips off null byte
  output->swap(temp);
  return true;
}

bool Base64::PaddingSkippedRfcWebSafeEncode(const StringPiece& input,
                                            std::string* output) {
  std::string output_with_padding;
  if (!RfcWebSafeEncode(input, &output_with_padding))
    return false;
  *output = StringTrimRight(output_with_padding, '=');
  return true;
}

bool Base64::Decode(const StringPiece& input, std::string* output) {
  std::string temp;
  temp.resize(modp_b64_decode_len(input.size()));

  // does not null terminate result since result is binary data!
  int input_size = static_cast<int>(input.size());
  int output_size = modp_b64_decode(&(temp[0]), input.data(), input_size);
  if (output_size < 0)
    return false;

  temp.resize(output_size);
  output->swap(temp);
  return true;
}

bool Base64::RfcWebSafeDecode(const StringPiece& input, std::string* output) {
  std::string temp;
  temp.resize(modp_b64r_decode_len(input.size()));

  // does not null terminate result since result is binary data!
  int input_size = static_cast<int>(input.size());
  int output_size = modp_b64r_decode(&(temp[0]), input.data(), input_size);
  if (output_size < 0)
    return false;

  temp.resize(output_size);
  output->swap(temp);
  return true;
}

bool Base64::PaddingSkippedRfcWebSafeDecode(const StringPiece& input,
                                            std::string* output) {
  std::string input_with_padding =
      StringConcat(input, std::string((4 - input.size() % 4) % 4, '='));
  return RfcWebSafeDecode(input_with_padding, output);
}
// } // namespace common
