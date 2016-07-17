// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>

#ifndef APP_QZAP_SERVICE_RETRIEVAL_ADINDEX_COMPRESSOR_H_
#define APP_QZAP_SERVICE_RETRIEVAL_ADINDEX_COMPRESSOR_H_

#include <stdint.h>
#include <string>

class Compressor
{
public:
    Compressor()
    {
    }
    virtual ~Compressor()
    {
    }

    virtual std::string Name() const = 0 ;
    virtual bool Compress(const std::string& uncompressed, std::string* compressed) const = 0;
    virtual bool Uncompress(const std::string& compressed, std::string* uncompressed) const = 0;

    static Compressor* GetCompressor(const std::string& name);
    static std::string UncompressedName()
    {
        return "uncompressed";
    }
    static std::string DebugString();
};

#endif  //  APP_QZAP_SERVICE_RETRIEVAL_ADINDEX_COMPRESSOR_H_
