// Copyright (c) 1998-2014 Tencent Technology Inc. All rights reserved.
// authurguo <authurguo@tencent.com> (2016.1 - *)
//
#include <string>
#include "common/crypto/rsa/rsa.h"
#include "thirdparty/gtest/gtest.h"

using gdt::RsaKey;
using gdt::Rsa;

TEST(RsaEncryptorSuite, PrivateDecryptCase) {
  std::string from =
      "Zm7AXdoahinZP3YsdYPdeIRDacVL1uC31nUcErEnCiusxIV7817iTyrAVFWeiD+2"
      "YRffLRC7hDfRkAY4xqQvNMGKZLa/DMc1LNs47WBoaPhucxuC7BIOVHMKMAk2NL47"
      "ispV2toz8HeMf2IAunwfJQur4seyEl5NSu19c+X4ncF4xab8uumzB3hfBudRy3hn"
      "MHbufJ35mj34FuKKCocqCgVzXcph2dWENTVyGw7cTWQXzdg1vWudWuSzC10Fy6sl"
      "yOGysp76UyZiuzx3ikb6dnkXe4nNvpAoYVvrEpvWYTAbAjTSlDYqXJ9KpQLBapf0"
      "39h97013sy5aUkcc2RlK6Q==";
  std::string to, empty;
  RsaKey decrypting_key;

  EXPECT_FALSE(Rsa::PrivateDecryptFromBase64(
      from, &to, &decrypting_key, RSA_PKCS1_PADDING));

  ASSERT_TRUE(decrypting_key.Create());
  ASSERT_TRUE(decrypting_key.ReadPrivatePemFile("test_data/private_key.pem"));

  EXPECT_FALSE(Rsa::PrivateDecryptFromBase64(
      empty, &to, &decrypting_key, RSA_PKCS1_PADDING));
  EXPECT_FALSE(Rsa::PrivateDecryptFromBase64(
      from, NULL, &decrypting_key, RSA_PKCS1_PADDING));
  EXPECT_TRUE(Rsa::PrivateDecryptFromBase64(
      from, &to, &decrypting_key, RSA_PKCS1_PADDING));
  EXPECT_EQ("I have a dream there is no AD!", to);
}

TEST(RsaEncryptorSuite, PublicEncryptCase) {
  std::string from = "I have a dream there is no AD!";
  std::string to, empty;
  RsaKey encrypting_key;

  EXPECT_FALSE(Rsa::PublicEncryptToBase64(
      from, &to, &encrypting_key, RSA_PKCS1_PADDING));

  ASSERT_TRUE(encrypting_key.Create());
  ASSERT_TRUE(encrypting_key.ReadPublicPemFile("test_data/public_key.pem"));
  EXPECT_TRUE(Rsa::PublicEncryptToBase64(
      empty, &to, &encrypting_key, RSA_PKCS1_PADDING));
  EXPECT_FALSE(Rsa::PublicEncryptToBase64(
      from, NULL, &encrypting_key, RSA_PKCS1_PADDING));
  EXPECT_TRUE(Rsa::PublicEncryptToBase64(
      from, &to, &encrypting_key, RSA_PKCS1_PADDING));

  std::string result;
  RsaKey decrypting_key;

  ASSERT_TRUE(decrypting_key.Create());
  ASSERT_TRUE(decrypting_key.ReadPrivatePemFile("test_data/private_key.pem"));
  ASSERT_TRUE(Rsa::PrivateDecryptFromBase64(
      to, &result, &decrypting_key, RSA_PKCS1_PADDING));
  ASSERT_EQ(from, result);
}

TEST(RsaEncryptorSuite, PubEncryptCase) {
  std::string from = "I have a dream there is no AD!";
  std::string to, empty;
  RsaKey encrypting_key;

  EXPECT_FALSE(Rsa::PublicEncryptToBase64(
      from, &to, &encrypting_key, RSA_PKCS1_PADDING));

  ASSERT_TRUE(encrypting_key.Create());
  ASSERT_TRUE(encrypting_key.ReadPUBPemFile("test_data/PUB_key.pem"));
  EXPECT_TRUE(Rsa::PublicEncryptToBase64(
      empty, &to, &encrypting_key, RSA_PKCS1_PADDING));
  EXPECT_FALSE(Rsa::PublicEncryptToBase64(
      from, NULL, &encrypting_key, RSA_PKCS1_PADDING));
  EXPECT_TRUE(Rsa::PublicEncryptToBase64(
      from, &to, &encrypting_key, RSA_PKCS1_PADDING));

  std::string result;
  RsaKey decrypting_key;

  ASSERT_TRUE(decrypting_key.Create());
  ASSERT_TRUE(decrypting_key.ReadPrivatePemFile("test_data/private_key.pem"));
  ASSERT_TRUE(Rsa::PrivateDecryptFromBase64(
      to, &result, &decrypting_key, RSA_PKCS1_PADDING));
  ASSERT_EQ(from, result);
}
