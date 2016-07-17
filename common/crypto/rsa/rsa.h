// Copyright (c) 1998-2014 Tencent Technology Inc. All rights reserved.
// authurguo <authurguo@tencent.com> (2016.1 - *)
//
#ifndef COMMON_CRYPTO_RSA_RSA_H_
#define COMMON_CRYPTO_RSA_RSA_H_

#include <string>
#include "common/base/string/string_piece.h"
#include "common/crypto/rsa/rsa_key.h"
#include "thirdparty/openssl/err.h"

namespace gdt {

// @class Rsa
// @brief Wrapper of rsa encrypting function in openssl crypto lib
class Rsa {
 public:
  // @function PublicEncrypt
  // @brief Wrapper of the function RSA_public_encrypt in openssl/rsa.h
  static bool PublicEncrypt(
      const StringPiece& from, std::string* to, RsaKey* key, int32_t padding);

  static bool PublicEncryptToBase64(
      const StringPiece& from, std::string* to, RsaKey* key, int32_t padding);

  // @function PrivateDecrypt
  // @brief Wrapper of the function RSA_private_decrypt in openssl/rsa.h
  static bool PrivateDecrypt(
      const StringPiece& from, std::string* to, RsaKey* key, int32_t padding);

  static bool PrivateDecryptFromBase64(
      const StringPiece& from, std::string* to, RsaKey* key, int32_t padding);
};  // class Rsa

}  // namespace gdt
#endif  // COMMON_CRYPTO_RSA_RSA_H_
