#include <arpa/inet.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>

#include "app/qzap/common/encrypt/tea/tea.h"


static int32_t GetCurrentProcessId()
{
    return getpid();
}

static uint32_t GetTickCount()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0)
    {
        return 0;
    }
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

static int32_t xrand()
{
    static int32_t holdrand = GetCurrentProcessId() * 100 + GetTickCount();
    return ( (holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff;
}


static const uint32_t kDelta = 0x9e3779b9;

static const int32_t kRounds = 16;
static const int32_t kLogRounds = 4;
static const int32_t kSaltLen = 2;
static const int32_t kZeroLen = 7;

/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
static void TeaEncryptECB(const char *pInBuf, const char *pKey, char *pOutBuf)
{
    uint32_t y, z;
    uint32_t sum;
    uint32_t k[4];
    int32_t i;

    /*plain-text is TCP/IP-endian;*/

    /*GetBlockBigEndian(in, y, z);*/
    y = ntohl(*((uint32_t*)pInBuf));
    z = ntohl(*((uint32_t*)(pInBuf+4)));
    /*TCP/IP network byte order (which is big-endian).*/

    for ( i = 0; i<4; i++)
    {
        /*now key is TCP/IP-endian;*/
        k[i] = ntohl(*((uint32_t*)(pKey+i*4)));
    }

    sum = 0;
    for (i=0; i<kRounds; i++)
    {
        sum += kDelta;
        y += ((z << 4) + k[0]) ^ (z + sum) ^ ((z >> 5) + k[1]);
        z += ((y << 4) + k[2]) ^ (y + sum) ^ ((y >> 5) + k[3]);
    }



    *((uint32_t*)pOutBuf) = htonl(y);
    *((uint32_t*)(pOutBuf+4)) = htonl(z);


    /*now encrypted buf is TCP/IP-endian;*/
}

/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
static void TeaDecryptECB(const char *pInBuf, const char *pKey, char *pOutBuf)
{
    uint32_t y, z, sum;
    uint32_t k[4];
    int32_t i;

    /*now encrypted buf is TCP/IP-endian;*/
    /*TCP/IP network byte order (which is big-endian).*/
    y = ntohl(*((uint32_t*)pInBuf));
    z = ntohl(*((uint32_t*)(pInBuf+4)));

    for ( i=0; i<4; i++)
    {
        /*key is TCP/IP-endian;*/
        k[i] = ntohl(*((uint32_t*)(pKey+i*4)));
    }

    sum = kDelta << kLogRounds;
    for (i=0; i<kRounds; i++)
    {
        z -= ((y << 4) + k[2]) ^ (y + sum) ^ ((y >> 5) + k[3]);
        y -= ((z << 4) + k[0]) ^ (z + sum) ^ ((z >> 5) + k[1]);
        sum -= kDelta;
    }

    *((uint32_t*)pOutBuf) = htonl(y);
    *((uint32_t*)(pOutBuf+4)) = htonl(z);

    /*now plain-text is TCP/IP-endian;*/
}




/* ///////////////////////////////////////////////////////////////////////////////////////////// */

/// QQ对称计算加密长度第二代函数. (TEA加密算法,CBC模式). 密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
/** @param nInBufLen in,    nInBufLen为需加密的明文部分(Body)长度
@return int,            返回为加密后的长度(是8byte的倍数)
*/
/*pKey为16byte*/
/*
输入:nInBufLen为需加密的明文部分(Body)长度;
输出:返回为加密后的长度(是8byte的倍数);
*/
/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
static int32_t oi_symmetry_encrypt2_len(int nInBufLen)
{

    int32_t nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero的长度*/;
    int32_t nPadlen;

    /*根据Body长度计算PadLen,最小必需长度必需为8byte的整数倍*/
    nPadSaltBodyZeroLen = nInBufLen/*Body长度*/+1+kSaltLen+kZeroLen/*PadLen(1byte)+Salt(2byte)+Zero(7byte)*/;
    nPadlen=nPadSaltBodyZeroLen%8;
    if (nPadlen) /*len=nSaltBodyZeroLen%8*/
    {
        /*模8余0需补0,余1补7,余2补6,...,余7补1*/
        nPadlen=8-nPadlen;
    }

    return nPadSaltBodyZeroLen+nPadlen;
}


/// QQ对称加密第二代函数. (TEA加密算法,CBC模式). 密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
/** @param pInBuf in,        需加密的明文部分(Body)
@param nInBufLen in,    pInBuf长度
@param pKey in,            加密Key, 长度固定为16Byte.
@param pOutBuf out,        输出的密文
@param pOutBufLen in out,    pOutBuf的长度. 长度是8byte的倍数,至少应预留nInBufLen+17;
*/
/*pKey为16byte*/
/*
输入:pInBuf为需加密的明文部分(Body),nInBufLen为pInBuf长度;
输出:pOutBuf为密文格式,pOutBufLen为pOutBuf的长度是8byte的倍数;
*/
/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
static void oi_symmetry_encrypt2(const char* pInBuf, int32_t nInBufLen, const char* pKey, char* pOutBuf, int32_t *pOutBufLen)
{

    int32_t nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero的长度*/;
    int32_t nPadlen;
    char src_buf[8], iv_plain[8], *iv_crypt;
    int32_t src_i, i, j;

    /*根据Body长度计算PadLen,最小必需长度必需为8byte的整数倍*/
    nPadSaltBodyZeroLen = nInBufLen/*Body长度*/+1+kSaltLen+kZeroLen/*PadLen(1byte)+Salt(2byte)+Zero(7byte)*/;
    nPadlen=nPadSaltBodyZeroLen%8;
    if (nPadlen) /*len=nSaltBodyZeroLen%8*/
    {
        /*模8余0需补0,余1补7,余2补6,...,余7补1*/
        nPadlen=8-nPadlen;
    }

    /*加密第一块数据(8byte),取前面10byte*/
    src_buf[0] = (((char)xrand()) & 0x0f8/*最低三位存PadLen,清零*/) | (char)nPadlen;
    src_i = 1; /*src_i指向src_buf下一个位置*/

    while (nPadlen--)
        src_buf[src_i++]=(char)xrand(); /*Padding*/

    /*come here, src_i must <= 8*/

    for ( i=0; i<8; i++)
        iv_plain[i] = 0;
    iv_crypt = iv_plain; /*make zero iv*/

    *pOutBufLen = 0; /*init OutBufLen*/

    for (i=1;i<=kSaltLen;) /*Salt(2byte)*/
    {
        if (src_i<8)
        {
            src_buf[src_i++]=(char)xrand();
            i++; /*i inc in here*/
        }

        if (src_i==8)
        {
            /*src_i==8*/

            for (j=0;j<8;j++) /*加密前异或前8个byte的密文(iv_crypt指向的)*/
                src_buf[j]^=iv_crypt[j];

            /*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
            /*加密*/
            TeaEncryptECB(src_buf, pKey, pOutBuf);

            for (j=0;j<8;j++) /*加密后异或前8个byte的明文(iv_plain指向的)*/
                pOutBuf[j]^=iv_plain[j];

            /*保存当前的iv_plain*/
            for (j=0;j<8;j++)
                iv_plain[j]=src_buf[j];

            /*更新iv_crypt*/
            src_i=0;
            iv_crypt=pOutBuf;
            *pOutBufLen+=8;
            pOutBuf+=8;
        }
    }

    /*src_i指向src_buf下一个位置*/

    while (nInBufLen)
    {
        if (src_i<8)
        {
            src_buf[src_i++]=*(pInBuf++);
            nInBufLen--;
        }

        if (src_i==8)
        {
            /*src_i==8*/

            for (j=0;j<8;j++) /*加密前异或前8个byte的密文(iv_crypt指向的)*/
                src_buf[j]^=iv_crypt[j];
            /*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
            TeaEncryptECB(src_buf, pKey, pOutBuf);

            for (j=0;j<8;j++) /*加密后异或前8个byte的明文(iv_plain指向的)*/
                pOutBuf[j]^=iv_plain[j];

            /*保存当前的iv_plain*/
            for (j=0;j<8;j++)
                iv_plain[j]=src_buf[j];

            src_i=0;
            iv_crypt=pOutBuf;
            *pOutBufLen+=8;
            pOutBuf+=8;
        }
    }

    /*src_i指向src_buf下一个位置*/

    for (i=1;i<=kZeroLen;)
    {
        if (src_i<8)
        {
            src_buf[src_i++]=0;
            i++; /*i inc in here*/
        }

        if (src_i==8)
        {
            /*src_i==8*/

            for (j=0;j<8;j++) /*加密前异或前8个byte的密文(iv_crypt指向的)*/
                src_buf[j]^=iv_crypt[j];
            /*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
            TeaEncryptECB(src_buf, pKey, pOutBuf);

            for (j=0;j<8;j++) /*加密后异或前8个byte的明文(iv_plain指向的)*/
                pOutBuf[j]^=iv_plain[j];

            /*保存当前的iv_plain*/
            for (j=0;j<8;j++)
                iv_plain[j]=src_buf[j];

            src_i=0;
            iv_crypt=pOutBuf;
            *pOutBufLen+=8;
            pOutBuf+=8;
        }
    }

}

/// QQ对称解密第二代函数. (TEA加密算法,CBC模式). 密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
/** @param pInBuf in,        需解密的密文.
@param nInBufLen in,    pInBuf长度. 8byte的倍数
@param pKey in,            解密Key, 长度固定为16Byte.
@param pOutBuf out,        输出的明文
@param pOutBufLen in out,    pOutBuf的长度. 至少应预留nInBufLen-10
@return bool,            如果格式正确返回TRUE
*/

/*pKey为16byte*/
/*
输入:pInBuf为密文格式,nInBufLen为pInBuf的长度是8byte的倍数; *pOutBufLen为接收缓冲区的长度
特别注意*pOutBufLen应预置接收缓冲区的长度!
输出:pOutBuf为明文(Body),pOutBufLen为pOutBuf的长度,至少应预留nInBufLen-10;
返回值:如果格式正确返回true;
*/
/*TEA解密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
static bool oi_symmetry_decrypt2(const char* pInBuf, int32_t nInBufLen, const char* pKey, char* pOutBuf, int32_t *pOutBufLen)
{

    int32_t nPadLen, nPlainLen;
    char dest_buf[8], zero_buf[8];
    const char *iv_pre_crypt, *iv_cur_crypt;
    int32_t dest_i, i, j;
    const char *pInBufBoundary;
    int32_t nBufPos;
    nBufPos = 0;



    if ((nInBufLen%8) || (nInBufLen<16)) return false;


    TeaDecryptECB(pInBuf, pKey, dest_buf);

    nPadLen = dest_buf[0] & 0x7/*只要最低三位*/;

    /*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
    i = nInBufLen-1/*PadLen(1byte)*/-nPadLen-kSaltLen-kZeroLen; /*明文长度*/
    if ((*pOutBufLen<i) || (i<0)) return false;
    *pOutBufLen = i;

    pInBufBoundary = pInBuf + nInBufLen; /*输入缓冲区的边界，下面不能pInBuf>=pInBufBoundary*/


    for ( i=0; i<8; i++)
        zero_buf[i] = 0;

    iv_pre_crypt = zero_buf;
    iv_cur_crypt = pInBuf; /*init iv*/

    pInBuf += 8;
    nBufPos += 8;

    dest_i=1; /*dest_i指向dest_buf下一个位置*/


    /*把Padding滤掉*/
    dest_i+=nPadLen;

    /*dest_i must <=8*/

    /*把Salt滤掉*/
    for (i=1; i<=kSaltLen;)
    {
        if (dest_i<8)
        {
            dest_i++;
            i++;
        }
        else if (dest_i==8)
        {
            /*解开一个新的加密块*/

            /*改变前一个加密块的指针*/
            iv_pre_crypt = iv_cur_crypt;
            iv_cur_crypt = pInBuf;

            /*异或前一块明文(在dest_buf[]中)*/
            for (j=0; j<8; j++)
            {
                if ( (nBufPos + j) >= nInBufLen)
                    return false;
                dest_buf[j]^=pInBuf[j];
            }

            /*dest_i==8*/
            TeaDecryptECB(dest_buf, pKey, dest_buf);

            /*在取出的时候才异或前一块密文(iv_pre_crypt)*/


            pInBuf += 8;
            nBufPos += 8;

            dest_i=0; /*dest_i指向dest_buf下一个位置*/
        }
    }

    /*还原明文*/

    nPlainLen=*pOutBufLen;
    while (nPlainLen)
    {
        if (dest_i<8)
        {
            *(pOutBuf++)=dest_buf[dest_i]^iv_pre_crypt[dest_i];
            dest_i++;
            nPlainLen--;
        }
        else if (dest_i==8)
        {
            /*dest_i==8*/

            /*改变前一个加密块的指针*/
            iv_pre_crypt = iv_cur_crypt;
            iv_cur_crypt = pInBuf;

            /*解开一个新的加密块*/

            /*异或前一块明文(在dest_buf[]中)*/
            for (j=0; j<8; j++)
            {
                if ( (nBufPos + j) >= nInBufLen)
                    return false;
                dest_buf[j]^=pInBuf[j];
            }

            TeaDecryptECB(dest_buf, pKey, dest_buf);

            /*在取出的时候才异或前一块密文(iv_pre_crypt)*/


            pInBuf += 8;
            nBufPos += 8;

            dest_i=0; /*dest_i指向dest_buf下一个位置*/
        }
    }

    /*校验Zero*/
    for (i=1;i<=kZeroLen;)
    {
        if (dest_i<8)
        {
            if (dest_buf[dest_i]^iv_pre_crypt[dest_i]) return false;
            dest_i++;
            i++;
        }
        else if (dest_i==8)
        {
            /*改变前一个加密块的指针*/
            iv_pre_crypt = iv_cur_crypt;
            iv_cur_crypt = pInBuf;

            /*解开一个新的加密块*/

            /*异或前一块明文(在dest_buf[]中)*/
            for (j=0; j<8; j++)
            {
                if ( (nBufPos + j) >= nInBufLen)
                    return false;
                dest_buf[j]^=pInBuf[j];
            }

            TeaDecryptECB(dest_buf, pKey, dest_buf);

            /*在取出的时候才异或前一块密文(iv_pre_crypt)*/
            pInBuf += 8;
            nBufPos += 8;
            dest_i=0; /*dest_i指向dest_buf下一个位置*/
        }

    }

    return true;
}

namespace tea {
void Encrypt(const std::string& plain_text, const std::string& key, std::string* encrypted_text)
{
    encrypted_text->clear();
    encrypted_text->resize(oi_symmetry_encrypt2_len(plain_text.length()));

    int32_t encrypt_len = 0;

    oi_symmetry_encrypt2(plain_text.c_str(), plain_text.length(),
                         key.c_str(),
                         const_cast<char*>(encrypted_text->c_str()),
                         &encrypt_len);
    encrypted_text->resize(encrypt_len);
}

bool Decrypt(const std::string& encrypted_text, const std::string& key, std::string* plain_text)
{
    if (key.length() != 16)
    {
        return false;
    }
    plain_text->clear();
    plain_text->resize(encrypted_text.length());
    if (encrypted_text.empty())
    {
        return true;
    }
    int32_t plain_len = plain_text->length();

    if (!oi_symmetry_decrypt2(encrypted_text.c_str(), encrypted_text.length(),
                             key.c_str(),
                             const_cast<char*>(plain_text->c_str()), &plain_len))
    {
        plain_text->clear();
        return false;
    }
    plain_text->resize(plain_len);
    return true;

}
} // namespace tea
