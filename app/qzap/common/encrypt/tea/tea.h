
#ifndef APP_QZAP_COMMON_ENCRYPT_TEA_TEA_H_
#define APP_QZAP_COMMON_ENCRYPT_TEA_TEA_H_
#include <string>

// ��װ�˼�ͨ��TEA�����㷨�� ����չʾcgi����sext
namespace tea {
/// QQ�ԳƼ��ܵڶ�������. (TEA�����㷨,CBCģʽ). ���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
void Encrypt(const std::string& plain_text, const std::string& key, std::string* encrypted_text);

/// QQ�Գƽ��ܵڶ�������. (TEA�����㷨,CBCģʽ). ���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
bool Decrypt(const std::string& encrypted_text, const std::string& key, std::string* plain_text);
} // namespae tea

#endif  // APP_QZAP_COMMON_ENCRYPT_TEA_TEA_H_
