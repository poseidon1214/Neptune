/**
 * @file BlockCompressionCodecQuicklz.h
 * @brief
 * @author kypoyin
 * @date 2010-05-05
 */

#ifndef BLOCK_COMPRESSION_CODEC_QUICKLZ_H__
#define BLOCK_COMPRESSION_CODEC_QUICKLZ_H__

#include "app/qzap/common/compress/block_compression_codec.h"

namespace common {

class BlockCompressionCodecQuicklz : public BlockCompressionCodec {
public:
    BlockCompressionCodecQuicklz();
    virtual ~BlockCompressionCodecQuicklz();

    virtual int GetType() { return QUICKLZ; }

private:
    virtual int DoDeflate(const char *input,
                          size_t input_size,
                          char *output,
                          size_t *output_size);

    virtual int DoInflate(const char *input,
                          size_t input_size,
                          char *output,
                          size_t *output_size);

    uint8_t *m_workmem;
};

} // namespace common

#endif  // BLOCK_COMPRESSION_CODEC_QUICKLZ_H__
