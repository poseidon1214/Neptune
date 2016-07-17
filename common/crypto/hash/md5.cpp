// Copyright (c) 2011, Tencent Inc. All rights reserved.
// ��������װ
// �㷨˵����
//          ����ժҪ(MD��MessageDigest�������ǽ��ɱ䳤�ȵı���M��Ϊ����ɢ�к������룬Ȼ���
//          ��һ���̶����ȵı�־H(M)��H(M)ͨ����Ϊ����ժҪ(MD)������Ҫ�������������
//          ͨ��˫������һ���������Կ�����Ͷ��Ƚ�����M�����ɢ�к���H�������H��M����MD��
//          ���ó������Կ��MD���м��ܣ������ܵ�MD׷���ڱ���M�ĺ��棬���͵����ܶˡ����ն���
//          ��ȥ׷���ڱ���M������ܵ�MD������֪��ɢ�к�������H(M)�������Լ�ӵ�е���ԿK�Լ���
//          ��MD���ܶ��ó���ʵ��MD���Ƚϼ������H(M)��MD����һ�£����յ��ı���M����ġ�

// GLOBAL_NOLINT(runtime/int)
// GLOBAL_NOLINT(runtime/references)
// GLOBAL_NOLINT(runtime/printf)

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "common/crypto/hash/md5.h"

namespace gdt {
void MD5::DoInit() {
  MD5_Init(&m_ctx);
}

void MD5::DoUpdate(const void* data, size_t size) {
  MD5_Update(&m_ctx, data, size);
}

void MD5::DoFinal(void* digest) {
  MD5_Final(reinterpret_cast<unsigned char*>(digest), &m_ctx);
}

void MD5::DoDigest(const void *data, size_t size, void *digest) {
  MD5 md5;
  md5.Update(data, size);
  md5.Final(digest);
}

}
