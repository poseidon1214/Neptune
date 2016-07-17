// Copyright (c) 1998-2014 Tencent Technology Inc. All rights reserved.
// authurguo <authurguo@tencent.com> (2016.1 - *)
//
#include "common/crypto/rsa/rsa_key.h"
#include "thirdparty/openssl/pem.h"

namespace gdt {

RsaKey::RsaKey() {
  rsa_key_ = NULL;
}

RsaKey::~RsaKey() {
  Clear();
}

bool RsaKey::Create() {
  if (!IsEmpty()) {
    return false;
  }
  if ((rsa_key_ = RSA_new()) == NULL) {
    return false;
  }
  return true;
}

void RsaKey::Clear() {
  if (!IsEmpty()) {
    RSA_free(rsa_key_);
    rsa_key_ = NULL;
  }
}

bool RsaKey::IsEmpty() const {
  return (rsa_key_ == NULL);
}

void RsaKey::Reset(RSA* key) {
  this->Clear();
  if (key != NULL) {
    rsa_key_ = key;
  }
}

RSA* RsaKey::Get() {
  return rsa_key_;
}

int RsaKey::Size() const {
  return RSA_size(rsa_key_);
}

bool RsaKey::CheckKey() const {
  return (RSA_check_key(rsa_key_) == 1);
}

bool RsaKey::ReadPublicPemFile(const StringPiece& pem_file_path) {
  if (pem_file_path.empty() || IsEmpty()) {
    return false;
  }

  // open pem file
  FILE* pem_file = NULL;
  if ((pem_file = fopen(pem_file_path.as_string().c_str(), "rb")) == NULL) {
    return false;
  }

  // read file
  bool is_succ = (
      PEM_read_RSAPublicKey(pem_file, &rsa_key_, NULL, NULL) != NULL);

  // close file
  if (pem_file != NULL) {
    fclose(pem_file);
    pem_file = NULL;
  }
  return is_succ;
}

bool RsaKey::ReadPUBPemFile(const StringPiece& pem_file_path) {
  if (pem_file_path.empty() || IsEmpty()) {
    return false;
  }

  // open pem file
  FILE* pem_file = NULL;
  if ((pem_file = fopen(pem_file_path.as_string().c_str(), "rb")) == NULL) {
    return false;
  }

  // read file
  bool is_succ = (
      PEM_read_RSA_PUBKEY(pem_file, &rsa_key_, NULL, NULL) != NULL);

  // close file
  if (pem_file != NULL) {
    fclose(pem_file);
    pem_file = NULL;
  }
  return is_succ;
}

bool RsaKey::ReadPrivatePemFile(const StringPiece& pem_file_path) {
  if (pem_file_path.empty() || IsEmpty()) {
    return false;
  }

  // open pem file
  FILE* pem_file = NULL;
  if ((pem_file = fopen(pem_file_path.as_string().c_str(), "rb")) == NULL) {
    return false;
  }

  // read file
  bool is_succ = (
      PEM_read_RSAPrivateKey(pem_file, &rsa_key_, NULL, NULL) != NULL);

  // close file
  if (pem_file != NULL) {
    fclose(pem_file);
    pem_file = NULL;
  }
  return is_succ;
}

}  // namespace gdt
