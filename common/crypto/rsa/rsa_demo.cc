// Copyright (c) 1998-2014 Tencent Technology Inc. All rights reserved.
// authurguo <authurguo@tencent.com> (2016.1 - *)
//
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include "common/crypto/rsa/rsa.h"
#include "common/crypto/rsa/rsa_key.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(method, "encode", "");
DEFINE_string(pem_file, "", "");
DEFINE_string(data, "", "");

bool RSAPublicEncode(
    const std::string& file_path, const std::string& from, std::string* to) {
  printf("====================\n");
  printf("RSA Encrypt\n");
  if (file_path.empty() || from.empty() || to == NULL) {
    printf("ERROR: Invalid params!\n");
    return false;
  }
  to->clear();

  bool is_encrypted = false;
  do {
    gdt::RsaKey key;
    if (!key.Create()) {
      printf("ERROR: Init RSA key fail!\n");
      is_encrypted = false;
      break;
    }
    if (!key.ReadPublicPemFile(file_path)) {
      printf("ERROR: Read public pem file fail!\n");
      is_encrypted = false;
      break;
    }
    if (!gdt::Rsa::PublicEncryptToBase64(from, to, &key, RSA_PKCS1_PADDING)) {
      printf("ERROR: Encrypt data fail!\n");
      is_encrypted = false;
      break;
    }
    is_encrypted = true;
  } while (false);

  CRYPTO_cleanup_all_ex_data();
  printf("====================\n");
  return is_encrypted;
}

bool RSAPUBEncode(
    const std::string& file_path, const std::string& from, std::string* to) {
  printf("====================\n");
  printf("RSA Encrypt\n");
  if (file_path.empty() || from.empty() || to == NULL) {
    printf("ERROR: Invalid params!\n");
    return false;
  }
  to->clear();

  bool is_encrypted = false;
  do {
    gdt::RsaKey key;
    if (!key.Create()) {
      printf("ERROR: Init RSA key fail!\n");
      is_encrypted = false;
      break;
    }
    if (!key.ReadPUBPemFile(file_path)) {
      printf("ERROR: Read PUB Pem file fail!\n");
      is_encrypted = false;
      break;
    }
    if (!gdt::Rsa::PublicEncryptToBase64(from, to, &key, RSA_PKCS1_PADDING)) {
      printf("ERROR: Encrypt data fail!\n");
      is_encrypted = false;
      break;
    }
    is_encrypted = true;
  } while (false);

  CRYPTO_cleanup_all_ex_data();
  printf("====================\n");
  return is_encrypted;
}

bool RSAPrivateDecode(
    const std::string& file_path, const std::string& from, std::string* to) {
  printf("====================\n");
  printf("RSA Decrypt\n");
  if (file_path.empty() || from.empty() || to == NULL) {
    printf("ERROR: Invalid params!\n");
    return false;
  }
  to->clear();

  bool is_encrypted = false;
  do {
    gdt::RsaKey key;
    if (!key.Create()) {
      printf("ERROR: Init RSA key fail!\n");
      is_encrypted = false;
      break;
    }
    if (!key.ReadPrivatePemFile(file_path)) {
      printf("ERROR: Read private Pem file fail! (%lu)\n", ERR_get_error());
      is_encrypted = false;
      break;
    }
    printf("STATUS: Read private Pem file successfully!\n");
    if (!gdt::Rsa::PrivateDecryptFromBase64(
        from, to, &key, RSA_PKCS1_PADDING)) {
      printf("ERROR: Decrypt data fail! (%lu)\n", ERR_get_error());
      is_encrypted = false;
      break;
    }
    printf("STATUS: Decrypt data successfully!\n");
    is_encrypted = true;
  } while (false);

  CRYPTO_cleanup_all_ex_data();
  printf("====================\n");
  return is_encrypted;
}

int32_t main(int32_t argc, char* args[]) {
  ::google::ParseCommandLineFlags(&argc, &args, false);
  std::string result;
  bool is_succ = false;
  if (FLAGS_method == "PUBEnc") {
    is_succ = RSAPUBEncode(FLAGS_pem_file, FLAGS_data, &result);
  } else if (FLAGS_method == "PubEnc") {
    is_succ = RSAPublicEncode(FLAGS_pem_file, FLAGS_data, &result);
  } else if (FLAGS_method == "PriDec") {
    is_succ = RSAPrivateDecode(FLAGS_pem_file, FLAGS_data, &result);
  } else {
    printf("Unknown method!\n");
  }
  if (is_succ) {
    printf("%s\n", result.c_str());
  }
  return 0;
}
