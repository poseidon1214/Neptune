
#ifndef APP_QZAP_COMMON_ENCRYPT_TEA_TEA_H_
#define APP_QZAP_COMMON_ENCRYPT_TEA_TEA_H_
#include <string>

// 封装了即通的TEA加密算法， 用于展示cgi解密sext
namespace tea {
/// QQ对称加密第二代函数. (TEA加密算法,CBC模式). 密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
void Encrypt(const std::string& plain_text, const std::string& key, std::string* encrypted_text);

/// QQ对称解密第二代函数. (TEA加密算法,CBC模式). 密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
bool Decrypt(const std::string& encrypted_text, const std::string& key, std::string* plain_text);
} // namespae tea

#endif  // APP_QZAP_COMMON_ENCRYPT_TEA_TEA_H_
