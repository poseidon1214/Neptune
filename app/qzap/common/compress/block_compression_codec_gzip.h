// // Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Yu Shizhan <stanyu@tencent.com>
// Created: 08/12/11
// Description:

#ifndef COMMON_COMPRESS_BLOCK_COMPRESSION_CODEC_GZIP_H_
#define COMMON_COMPRESS_BLOCK_COMPRESSION_CODEC_GZIP_H_

#include "app/qzap/common/compress/block_compression_codec.h"

namespace common
{

class BlockCompressionCodecGzip : public BlockCompressionCodec
{
public:
    virtual int GetType() { return GZIP; }

private:
    // see document of inflateInit2 in /usr/include/zlib.h
    static const int ZLIB_MAX_WINDOW_BITS = 15;
    static const int ZLIB_AUTO_HEADER_DETECT = 32;

    virtual int DoDeflate(const char *input,
            size_t input_size,
            char* output,
            size_t *output_size);

    virtual int DoInflate(const char *input,
            size_t input_size,
            char* output,
            size_t *output_size);
};

} // namespace common

#endif // COMMON_COMPRESS_BLOCK_COMPRESSION_CODEC_GZIP_H_
