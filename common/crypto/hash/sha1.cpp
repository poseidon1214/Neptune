// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: CHEN Feng <phongchen@tencent.com>
// Date: Nov 28, 2010

#include "common/crypto/hash/sha1.h"

namespace gdt {

void SHA1::DoInit() {
  SHA1_Init(&m_info);
}

void SHA1::DoUpdate(const void* buffer, size_t length) {
  SHA1_Update(&m_info, buffer, length);
}

void SHA1::DoFinal(void* buffer) {
  SHA1_Final(static_cast<unsigned char*>(buffer), &m_info);
}

void SHA1::DoDigest(const void* buffer, size_t size, void* result) {
  SHA1 sha1;
  sha1.Update(buffer, size);
  sha1.Final(result);
}

}  // namespace gdt
