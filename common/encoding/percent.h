// Copyright 2011, Tencent Inc.
// All rights reserved.

/// @author XiaoDong Chen <donniechen@tencent.com>
///         CHEN Feng <phongchen@tencent.com>
/// @brief percent encoding process
/// @date Mar 31, 2011

#ifndef COMMON_ENCODING_PERCENT_H
#define COMMON_ENCODING_PERCENT_H

#include <ctype.h>
#include <string>
#include "common/base/string/string_piece.h"

namespace gdt {

/// @brief percent encoding, majorly for url
/// @see http://en.wikipedia.org/wiki/Percent-encoding
struct PercentEncoding {
 public:
  // Same as escape in javascript

  /// @brief ����׷������� output
  static void EncodeAppend(const StringPiece& input, std::string* output);

  /// @brief ��������� output
  static void EncodeTo(const StringPiece& input, std::string* output);

  /// @brief ���������滻ԭ��������
  static void Encode(std::string *str);

  /// @brief ���ر����Ľ��
  static std::string Encode(const StringPiece& input);

  // Same as encodeURI in javascript

  /// @brief ����׷������� output
  static void EncodeUriAppend(const StringPiece& input, std::string* output);

  /// @brief ��������� output
  static void EncodeUriTo(const StringPiece& input, std::string* output);

  /// @brief ���������滻ԭ��������
  static void EncodeUri(std::string *str);

  /// @brief ���ر����Ľ��
  static std::string EncodeUri(const StringPiece& input);

  // Same as encodeURIComponent in javascript

  /// @brief ����׷������� output
  static void EncodeUriComponentAppend(const StringPiece& input,
                                       std::string* output);

  /// @brief ��������� output
  static void EncodeUriComponentTo(const StringPiece& input, std::string* output);

  /// @brief ���������滻ԭ��������
  static void EncodeUriComponent(std::string *str);

  /// @brief ���ر����Ľ��
  static std::string EncodeUriComponent(const StringPiece& input);


  /// @brief �����׷�ӷ�ʽ����� output ��
  static bool DecodeAppend(const StringPiece& input, std::string* output);

  /// @brief ���������� output ��
  /// @return �Ƿ�ɹ�
  static bool DecodeTo(const StringPiece& input, std::string* output);

  /// @brief ���������滻ԭ��������
  static bool Decode(std::string* str);
};

} // namespace common

#endif // COMMON_ENCODING_PERCENT_H
