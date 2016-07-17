// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>
// Date: Nov 28, 2010

#ifndef COMMON_CRYPTO_HASH_SHA1_H_
#define COMMON_CRYPTO_HASH_SHA1_H_

#include <stddef.h>
#include <string.h>
#include <string>
#include "common/crypto/hash/crypto_hash_base.h"
#include "thirdparty/openssl/sha.h"

namespace gdt {

/// SHA1 hash alogrithm
/// See CryptoHashBase for more interfaces
class SHA1 : public CryptoHashBase<SHA1> {
 public:
  static const size_t kDigestLength = SHA_DIGEST_LENGTH;

 public:
  SHA1() {
    Init();
  }

 private:
  // All CryptoHashBase call the following methods.
  friend class CryptoHashBase<SHA1>;

  void DoInit();
  void DoUpdate(const void* buffer, size_t length);
  void DoFinal(void* buffer);
  static void DoDigest(const void* buffer, size_t size, void* result);

 private:
  SHA_CTX m_info;
};

}  // namespace gdt

#endif  // COMMON_CRYPTO_HASH_SHA1_H_
