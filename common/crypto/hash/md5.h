// Copyright (c) 2008, Tencent Inc. All rights reserved.
// 版权信息：(C) 2003, Jerry.CS.HUST.China
// 作者：    Jerry

// GLOBAL_NOLINT(runtime/int)
// GLOBAL_NOLINT(runtime/references)

#ifndef COMMON_CRYPTO_HASH_MD5_H_
#define COMMON_CRYPTO_HASH_MD5_H_

#include <stddef.h>
#include <string.h>
#include <string>
#include "common/base/static_assert.h"
#include "common/crypto/hash/crypto_hash_base.h"
#include "thirdparty/openssl/md5.h"

namespace gdt {

/// See CryptoHashBase for more interfaces
class MD5 : public CryptoHashBase<MD5> {
 public:
  static const size_t kDigestLength = 16;

 public:
  MD5() {
    Init();
  }

 public:
  //////////////////////////////////////////////////////////////////////////
  // All following method are deprecated, you should call the methods defined
  // in CryptoHashBase.

  /// 外部使用的哈希函数：生成 16 bytes 的 hash 值

 private:
  // All CryptoHashBase call the following methods.
  friend class CryptoHashBase<MD5>;
  void DoInit();
  void DoUpdate(const void* data, size_t size);
  void DoFinal(void* digest);
  static void DoDigest(const void *data, size_t size, void *digest);

 private:
  MD5_CTX m_ctx;
};

}  // namespace gdt
#endif  // COMMON_CRYPTO_HASH_MD5_H_

