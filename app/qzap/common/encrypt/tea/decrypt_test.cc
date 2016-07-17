#include <string>

#include "thirdparty/gtest/gtest.h"

#include "app/qzap/common/encrypt/tea/tea.h"

using std::string;

TEST(TeaTest, decrypt_encrypt_match)
{
    string plain = "{\"req\":{\"friendqq\":1342727403}";
    string key = "1234567887654321";
    string cipher;
    tea::Encrypt(plain, key, &cipher);

    string decorded;
    bool ret = tea::Decrypt(cipher, key, &decorded);

    EXPECT_TRUE(ret);
    EXPECT_EQ(decorded, plain);
}

TEST(TeaTest, decrypt_encrypt_with_invalid_key)
{
    string plain = "{\"req\":{\"friendqq\":1342727403}";
    string key = "1234567887654321";
    string cipher;
    tea::Encrypt(plain, key, &cipher);

    string decorded;
    string invalid_key = "1234567887654320";
    bool ret = tea::Decrypt(cipher, invalid_key , &decorded);

    EXPECT_FALSE(ret);
    EXPECT_TRUE(decorded.empty());
}
TEST(TeaTest, make_sure_decrypt_key_len_is_16)
{
    string plain = "hi";
    string key = "123456788765";
    string cipher;
    tea::Encrypt(plain, key, &cipher);

    string decorded;
    bool ret = tea::Decrypt(cipher, key, &decorded);

    EXPECT_FALSE(ret);
}

TEST(TeaTest, decrypt_empty_string_alway_success)
{
    string key = "1234567887654321";
    string cipher;

    string decorded;
    bool ret = tea::Decrypt(cipher, key, &decorded);

    EXPECT_TRUE(ret);
    EXPECT_TRUE(decorded.empty());
}
