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

/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
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

/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
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

/// QQ�ԳƼ�����ܳ��ȵڶ�������. (TEA�����㷨,CBCģʽ). ���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
/** @param nInBufLen in,    nInBufLenΪ����ܵ����Ĳ���(Body)����
@return int,            ����Ϊ���ܺ�ĳ���(��8byte�ı���)
*/
/*pKeyΪ16byte*/
/*
����:nInBufLenΪ����ܵ����Ĳ���(Body)����;
���:����Ϊ���ܺ�ĳ���(��8byte�ı���);
*/
/*TEA�����㷨,CBCģʽ*/
/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
static int32_t oi_symmetry_encrypt2_len(int nInBufLen)
{

    int32_t nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero�ĳ���*/;
    int32_t nPadlen;

    /*����Body���ȼ���PadLen,��С���賤�ȱ���Ϊ8byte��������*/
    nPadSaltBodyZeroLen = nInBufLen/*Body����*/+1+kSaltLen+kZeroLen/*PadLen(1byte)+Salt(2byte)+Zero(7byte)*/;
    nPadlen=nPadSaltBodyZeroLen%8;
    if (nPadlen) /*len=nSaltBodyZeroLen%8*/
    {
        /*ģ8��0�貹0,��1��7,��2��6,...,��7��1*/
        nPadlen=8-nPadlen;
    }

    return nPadSaltBodyZeroLen+nPadlen;
}


/// QQ�ԳƼ��ܵڶ�������. (TEA�����㷨,CBCģʽ). ���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
/** @param pInBuf in,        ����ܵ����Ĳ���(Body)
@param nInBufLen in,    pInBuf����
@param pKey in,            ����Key, ���ȹ̶�Ϊ16Byte.
@param pOutBuf out,        ���������
@param pOutBufLen in out,    pOutBuf�ĳ���. ������8byte�ı���,����ӦԤ��nInBufLen+17;
*/
/*pKeyΪ16byte*/
/*
����:pInBufΪ����ܵ����Ĳ���(Body),nInBufLenΪpInBuf����;
���:pOutBufΪ���ĸ�ʽ,pOutBufLenΪpOutBuf�ĳ�����8byte�ı���;
*/
/*TEA�����㷨,CBCģʽ*/
/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
static void oi_symmetry_encrypt2(const char* pInBuf, int32_t nInBufLen, const char* pKey, char* pOutBuf, int32_t *pOutBufLen)
{

    int32_t nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero�ĳ���*/;
    int32_t nPadlen;
    char src_buf[8], iv_plain[8], *iv_crypt;
    int32_t src_i, i, j;

    /*����Body���ȼ���PadLen,��С���賤�ȱ���Ϊ8byte��������*/
    nPadSaltBodyZeroLen = nInBufLen/*Body����*/+1+kSaltLen+kZeroLen/*PadLen(1byte)+Salt(2byte)+Zero(7byte)*/;
    nPadlen=nPadSaltBodyZeroLen%8;
    if (nPadlen) /*len=nSaltBodyZeroLen%8*/
    {
        /*ģ8��0�貹0,��1��7,��2��6,...,��7��1*/
        nPadlen=8-nPadlen;
    }

    /*���ܵ�һ������(8byte),ȡǰ��10byte*/
    src_buf[0] = (((char)xrand()) & 0x0f8/*�����λ��PadLen,����*/) | (char)nPadlen;
    src_i = 1; /*src_iָ��src_buf��һ��λ��*/

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

            for (j=0;j<8;j++) /*����ǰ���ǰ8��byte������(iv_cryptָ���)*/
                src_buf[j]^=iv_crypt[j];

            /*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
            /*����*/
            TeaEncryptECB(src_buf, pKey, pOutBuf);

            for (j=0;j<8;j++) /*���ܺ����ǰ8��byte������(iv_plainָ���)*/
                pOutBuf[j]^=iv_plain[j];

            /*���浱ǰ��iv_plain*/
            for (j=0;j<8;j++)
                iv_plain[j]=src_buf[j];

            /*����iv_crypt*/
            src_i=0;
            iv_crypt=pOutBuf;
            *pOutBufLen+=8;
            pOutBuf+=8;
        }
    }

    /*src_iָ��src_buf��һ��λ��*/

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

            for (j=0;j<8;j++) /*����ǰ���ǰ8��byte������(iv_cryptָ���)*/
                src_buf[j]^=iv_crypt[j];
            /*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
            TeaEncryptECB(src_buf, pKey, pOutBuf);

            for (j=0;j<8;j++) /*���ܺ����ǰ8��byte������(iv_plainָ���)*/
                pOutBuf[j]^=iv_plain[j];

            /*���浱ǰ��iv_plain*/
            for (j=0;j<8;j++)
                iv_plain[j]=src_buf[j];

            src_i=0;
            iv_crypt=pOutBuf;
            *pOutBufLen+=8;
            pOutBuf+=8;
        }
    }

    /*src_iָ��src_buf��һ��λ��*/

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

            for (j=0;j<8;j++) /*����ǰ���ǰ8��byte������(iv_cryptָ���)*/
                src_buf[j]^=iv_crypt[j];
            /*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
            TeaEncryptECB(src_buf, pKey, pOutBuf);

            for (j=0;j<8;j++) /*���ܺ����ǰ8��byte������(iv_plainָ���)*/
                pOutBuf[j]^=iv_plain[j];

            /*���浱ǰ��iv_plain*/
            for (j=0;j<8;j++)
                iv_plain[j]=src_buf[j];

            src_i=0;
            iv_crypt=pOutBuf;
            *pOutBufLen+=8;
            pOutBuf+=8;
        }
    }

}

/// QQ�Գƽ��ܵڶ�������. (TEA�����㷨,CBCģʽ). ���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)
/** @param pInBuf in,        ����ܵ�����.
@param nInBufLen in,    pInBuf����. 8byte�ı���
@param pKey in,            ����Key, ���ȹ̶�Ϊ16Byte.
@param pOutBuf out,        ���������
@param pOutBufLen in out,    pOutBuf�ĳ���. ����ӦԤ��nInBufLen-10
@return bool,            �����ʽ��ȷ����TRUE
*/

/*pKeyΪ16byte*/
/*
����:pInBufΪ���ĸ�ʽ,nInBufLenΪpInBuf�ĳ�����8byte�ı���; *pOutBufLenΪ���ջ������ĳ���
�ر�ע��*pOutBufLenӦԤ�ý��ջ������ĳ���!
���:pOutBufΪ����(Body),pOutBufLenΪpOutBuf�ĳ���,����ӦԤ��nInBufLen-10;
����ֵ:�����ʽ��ȷ����true;
*/
/*TEA�����㷨,CBCģʽ*/
/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
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

    nPadLen = dest_buf[0] & 0x7/*ֻҪ�����λ*/;

    /*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
    i = nInBufLen-1/*PadLen(1byte)*/-nPadLen-kSaltLen-kZeroLen; /*���ĳ���*/
    if ((*pOutBufLen<i) || (i<0)) return false;
    *pOutBufLen = i;

    pInBufBoundary = pInBuf + nInBufLen; /*���뻺�����ı߽磬���治��pInBuf>=pInBufBoundary*/


    for ( i=0; i<8; i++)
        zero_buf[i] = 0;

    iv_pre_crypt = zero_buf;
    iv_cur_crypt = pInBuf; /*init iv*/

    pInBuf += 8;
    nBufPos += 8;

    dest_i=1; /*dest_iָ��dest_buf��һ��λ��*/


    /*��Padding�˵�*/
    dest_i+=nPadLen;

    /*dest_i must <=8*/

    /*��Salt�˵�*/
    for (i=1; i<=kSaltLen;)
    {
        if (dest_i<8)
        {
            dest_i++;
            i++;
        }
        else if (dest_i==8)
        {
            /*�⿪һ���µļ��ܿ�*/

            /*�ı�ǰһ�����ܿ��ָ��*/
            iv_pre_crypt = iv_cur_crypt;
            iv_cur_crypt = pInBuf;

            /*���ǰһ������(��dest_buf[]��)*/
            for (j=0; j<8; j++)
            {
                if ( (nBufPos + j) >= nInBufLen)
                    return false;
                dest_buf[j]^=pInBuf[j];
            }

            /*dest_i==8*/
            TeaDecryptECB(dest_buf, pKey, dest_buf);

            /*��ȡ����ʱ������ǰһ������(iv_pre_crypt)*/


            pInBuf += 8;
            nBufPos += 8;

            dest_i=0; /*dest_iָ��dest_buf��һ��λ��*/
        }
    }

    /*��ԭ����*/

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

            /*�ı�ǰһ�����ܿ��ָ��*/
            iv_pre_crypt = iv_cur_crypt;
            iv_cur_crypt = pInBuf;

            /*�⿪һ���µļ��ܿ�*/

            /*���ǰһ������(��dest_buf[]��)*/
            for (j=0; j<8; j++)
            {
                if ( (nBufPos + j) >= nInBufLen)
                    return false;
                dest_buf[j]^=pInBuf[j];
            }

            TeaDecryptECB(dest_buf, pKey, dest_buf);

            /*��ȡ����ʱ������ǰһ������(iv_pre_crypt)*/


            pInBuf += 8;
            nBufPos += 8;

            dest_i=0; /*dest_iָ��dest_buf��һ��λ��*/
        }
    }

    /*У��Zero*/
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
            /*�ı�ǰһ�����ܿ��ָ��*/
            iv_pre_crypt = iv_cur_crypt;
            iv_cur_crypt = pInBuf;

            /*�⿪һ���µļ��ܿ�*/

            /*���ǰһ������(��dest_buf[]��)*/
            for (j=0; j<8; j++)
            {
                if ( (nBufPos + j) >= nInBufLen)
                    return false;
                dest_buf[j]^=pInBuf[j];
            }

            TeaDecryptECB(dest_buf, pKey, dest_buf);

            /*��ȡ����ʱ������ǰһ������(iv_pre_crypt)*/
            pInBuf += 8;
            nBufPos += 8;
            dest_i=0; /*dest_iָ��dest_buf��һ��λ��*/
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
