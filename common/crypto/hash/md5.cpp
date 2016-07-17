// Copyright (c) 2011, Tencent Inc. All rights reserved.
// 张立明封装
// 算法说明：
//          报文摘要(MD，MessageDigest）。它是将可变长度的报文M作为单向散列函数输入，然后得
//          出一个固定长度的标志H(M)。H(M)通常称为报文摘要(MD)，它主要用于下面情况：
//          通信双方共享一个常规的密钥。发送端先将报文M输入给散列函数H，计算出H（M）即MD，
//          再用常规的密钥对MD进行加密，将加密的MD追加在报文M的后面，发送到接受端。接收端先
//          除去追加在报文M后面加密的MD，用已知的散列函数计算H(M)，再用自己拥有的密钥K对加密
//          的MD解密而得出真实的MD；比较计算出得H(M)和MD，若一致，则收到的报文M是真的。

// GLOBAL_NOLINT(runtime/int)
// GLOBAL_NOLINT(runtime/references)
// GLOBAL_NOLINT(runtime/printf)

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "common/crypto/hash/md5.h"

namespace gdt {
void MD5::DoInit() {
  MD5_Init(&m_ctx);
}

void MD5::DoUpdate(const void* data, size_t size) {
  MD5_Update(&m_ctx, data, size);
}

void MD5::DoFinal(void* digest) {
  MD5_Final(reinterpret_cast<unsigned char*>(digest), &m_ctx);
}

void MD5::DoDigest(const void *data, size_t size, void *digest) {
  MD5 md5;
  md5.Update(data, size);
  md5.Final(digest);
}

}
