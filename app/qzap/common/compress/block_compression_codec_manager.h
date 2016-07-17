/**
 * @file  BlockCompressionCodecManager.h
 * @brief ѹ�����������
 * @version 1.0
 * @date    2010-07-12 20ʱ57��46��
 * @author  Kypoyin, kypoyin@tencent.com
 */

#ifndef BLOCK_COMPRESSION_CODEC_MANAGER_H__
#define BLOCK_COMPRESSION_CODEC_MANAGER_H__

#include  "common/system/concurrency/mutex.h"
#include  "app/qzap/common/compress/block_compression_codec.h"

namespace common {

class BlockCompressionCodecManager {
public:
    BlockCompressionCodecManager(int max_free_codec = 5);
    ~BlockCompressionCodecManager();

    BlockCompressionCodec *CreateBlockCompressionCodec(int codec_type);
    void DestroyBlockCompressionCodec(BlockCompressionCodec *codec);

private:
    // ÿ�����͵�ѹ������ĳ�פ�ڴ���������
    int m_max_free_codec_size;
    int m_free_codec_size_array[BlockCompressionCodec::COMPRESSION_TYPE_LIMIT];

    // ѹ�������������ά����
    BlockCompressionCodec **m_codec_array;
    gdt::SimpleMutex m_mutex;
};

} // namespace common

#endif  // BLOCK_COMPRESSION_CODEC_MANAGER_H__
