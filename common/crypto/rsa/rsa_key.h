// Copyright (c) 1998-2014 Tencent Technology Inc. All rights reserved.
// authurguo <authurguo@tencent.com> (2016.1 - *)
//
#ifndef COMMON_CRYPTO_RSA_RSA_KEY_H_
#define COMMON_CRYPTO_RSA_RSA_KEY_H_

#include <stdio.h>
#include <stdint.h>
#include "common/base/string/string_piece.h"
#include "thirdparty/openssl/rsa.h"  // Do not support 32bit app!!!

namespace gdt {

class RsaKey {
 public:
  RsaKey();
  ~RsaKey();

  // @method Create
  // @brief Wrapper of function RSA_new in openssl/rsa.h
  bool Create();

  // @method Clear
  // @brief Wrapper of function RSA_free in openssl/rsa.h
  void Clear();

  // @method IsEmpty
  bool IsEmpty() const;

  // @method Reset
  void Reset(RSA* key);

  // @method Get
  RSA* Get();

  // @method Size
  // @brief Wrapper of function RSA_size in openssl/rsa.h
  int  Size() const;

  // @method CheckKey
  // @brief Wrapper of function RSA_check_key in openssl/rsa.h
  bool CheckKey() const;

  // @method ReadPublicPemFile
  // @brief Wrapper of function PEM_read_RSAPublicKey in openssl/pem.h
  bool ReadPublicPemFile(const StringPiece& pem_file_path);

  // @method ReadPUBPemFile
  // @brief Wrapper of function PEM_read_RSA_PUBKEY in openssl/pem.h
  bool ReadPUBPemFile(const StringPiece& pem_file_path);

  // @method ReadPrivatePemFile
  // @brief Wrapper of function PEM_read_RSAPrivateKey in openssl/pem.h
  bool ReadPrivatePemFile(const StringPiece& pem_file_path);

 private:
  // noncopyable
  RsaKey(const RsaKey& key);
  const RsaKey& operator=(const RsaKey& key);

  RSA* rsa_key_;
};  // class RsaKey

}  // namespace gdt
#endif  // COMMON_CRYPTO_RSA_RSA_KEY_H_
