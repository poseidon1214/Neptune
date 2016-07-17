// Copyright (c) 1998-2014 Tencent Technology Inc. All rights reserved.
// authurguo <authurguo@tencent.com> (2016.1 - *)
//
#include <string>
#include "common/base/scoped_ptr.h"
#include "common/crypto/rsa/rsa.h"
#include "common/encoding/base64.h"

namespace gdt {

bool Rsa::PublicEncrypt(
    const StringPiece& from, std::string* to, RsaKey* key, int32_t padding) {
  if (to == NULL || key == NULL) {
    return false;
  }
  if (key->IsEmpty()) {
    return false;
  }

  // malloc buffer
  int32_t key_size = 0;
  if ((key_size = key->Size()) < 0) {
    return false;
  }
  scoped_array<unsigned char> buffer(new unsigned char[key_size+1]);

  // encypt
  bool is_succ = false;
  int32_t ret = RSA_public_encrypt(
      from.size(), reinterpret_cast<const unsigned char*>(from.data()),
      buffer.get(), key->Get(), padding);
  if (ret >= 0) {
    is_succ = true;
    to->assign(reinterpret_cast<const char*>(buffer.get()), ret);
  } else {
    is_succ = false;
    to->clear();
  }

  return is_succ;
}

bool Rsa::PrivateDecrypt(
    const StringPiece& from, std::string* to, RsaKey* key, int32_t padding) {
  if (to == NULL || key == NULL) {
    return false;
  }
  if (key->IsEmpty()) {
    return false;
  }

  // malloc buffer
  int32_t key_size = 0;
  if ((key_size = key->Size()) < 0) {
    return false;
  }
  scoped_array<unsigned char> buffer(new unsigned char[key_size+1]);

  // encypt
  bool is_succ = false;
  int32_t ret = RSA_private_decrypt(
      from.size(), reinterpret_cast<const unsigned char*>(from.data()),
      buffer.get(), key->Get(), padding);
  if (ret >= 0) {
    is_succ = true;
    to->assign(reinterpret_cast<const char*>(buffer.get()), ret);
  } else {
    is_succ = false;
    to->clear();
  }

  return is_succ;
}

bool Rsa::PublicEncryptToBase64(
    const StringPiece& from, std::string* to, RsaKey* key, int32_t padding) {
  if (to == NULL || key == NULL) {
    return false;
  }
  std::string encrypting;
  if (!PublicEncrypt(from, &encrypting, key, padding)) {
    return false;
  }
  if (!Base64::Encode(encrypting, to)) {
    return false;
  }
  return true;
}

bool Rsa::PrivateDecryptFromBase64(
    const StringPiece& from, std::string* to, RsaKey* key, int32_t padding) {
  if (to == NULL || key == NULL) {
    return false;
  }
  std::string encrypting;
  if (!Base64::Decode(from.as_string(), &encrypting)) {
    return false;
  }
  if (!PrivateDecrypt(encrypting, to, key, padding)) {
    return false;
  }
  return true;
}

}  // namespace gdt
