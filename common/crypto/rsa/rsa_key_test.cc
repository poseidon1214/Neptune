// Copyright (c) 1998-2014 Tencent Technology Inc. All rights reserved.
// authurguo <authurguo@tencent.com> (2016.1 - *)
//
#include "common/crypto/rsa/rsa_key.h"
#include "thirdparty/gtest/gtest.h"

using gdt::RsaKey;

TEST(RsaKeySuite, ReadPriPemFileCase) {
  RsaKey key;

  EXPECT_TRUE(key.IsEmpty());
  EXPECT_FALSE(key.ReadPrivatePemFile("test_data/private_key.pem"));

  key.Create();
  EXPECT_FALSE(key.ReadPrivatePemFile(""));
  key.Clear();

  key.Create();
  EXPECT_FALSE(key.ReadPrivatePemFile("test_data/nonexist.pem"));
  key.Clear();

  key.Create();
  EXPECT_FALSE(key.ReadPrivatePemFile("test_data/public_key.pem"));
  key.Clear();

  key.Create();
  EXPECT_TRUE(key.ReadPrivatePemFile("test_data/private_key.pem"));
  EXPECT_TRUE(!key.IsEmpty());
  EXPECT_GT(key.Size(), 0);
  EXPECT_TRUE(key.CheckKey());
  key.Clear();
}

TEST(RsaKeySuite, ReadPubPemFileCase) {
  RsaKey key;

  EXPECT_FALSE(key.ReadPUBPemFile("test_data/PUB_key.pem"));

  key.Create();
  EXPECT_FALSE(key.ReadPUBPemFile(""));
  key.Clear();

  key.Create();
  EXPECT_FALSE(key.ReadPUBPemFile("test_data/nonexist.pem"));
  key.Clear();

  key.Create();
  EXPECT_FALSE(key.ReadPUBPemFile("test_data/private_key.pem"));
  key.Clear();

  key.Create();
  EXPECT_TRUE(key.ReadPUBPemFile("test_data/PUB_key.pem"));
  EXPECT_TRUE(!key.IsEmpty());
  EXPECT_GT(key.Size(), 0);
  key.Clear();
}

TEST(RsaKeySuite, ReadPublicPemFileCase) {
  RsaKey key;

  EXPECT_FALSE(key.ReadPublicPemFile("test_data/public_key.pem"));

  key.Create();
  EXPECT_FALSE(key.ReadPublicPemFile(""));
  key.Clear();

  key.Create();
  EXPECT_FALSE(key.ReadPublicPemFile("test_data/nonexist.pem"));
  key.Clear();

  key.Create();
  EXPECT_FALSE(key.ReadPublicPemFile("test_data/private_key.pem"));
  key.Clear();

  key.Create();
  EXPECT_TRUE(key.ReadPublicPemFile("test_data/public_key.pem"));
  EXPECT_TRUE(!key.IsEmpty());
  EXPECT_GT(key.Size(), 0);
  key.Clear();
}
