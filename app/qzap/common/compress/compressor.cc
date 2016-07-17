// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>
#include "app/qzap/common/compress/compressor.h"

#include <algorithm>
#include <string>

#include "app/qzap/common/base/class_register.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/zlib/zlib.h"


CLASS_REGISTER_DEFINE_REGISTRY(compressor_register, Compressor);

#define REGISTER_COMPRESSOR(name, class_name) \
    CLASS_REGISTER_OBJECT_CREATOR( \
        compressor_register, \
        Compressor, \
        name, \
        class_name)

#define CREATE_COMPRESSOR(name_as_string) \
    CLASS_REGISTER_CREATE_OBJECT(compressor_register, name_as_string)
DEFINE_int32(default_uncompress_ratio, 6, "");
DEFINE_uint64(max_uncompress_buffer_size, 8ull << 30 , "");
using std::string;

Compressor* Compressor::GetCompressor(const std::string& name)
{
    Compressor* compressor = CREATE_COMPRESSOR(name);
    if (compressor == NULL)
    {
        LOG(ERROR) << "Unknow compress method = [" << name << "]"
                   << DebugString();
    }
    return compressor;
}
std::string Compressor::DebugString()
{
    std::string output("Supported Compress method list=[");
    for (uint32_t i = 0;
         i < CLASS_REGISTER_CREATOR_COUNT(compressor_register);
         i++)
    {
        if (i != 0)
        {
            output.append(",");
        }
        output.append(CLASS_REGISTER_CREATOR_NAME(compressor_register, i));
    }
    output.append("]");
    return output;

}
class ZlibCompressor : public Compressor
{
public:
    ZlibCompressor()
    {
    }
    virtual ~ZlibCompressor()
    {
    }
    virtual std::string Name() const
    {
        return "zlib";
    }
    virtual bool Compress(const std::string& uncompressed, std::string* compressed) const;
    virtual bool Uncompress(const std::string& compressed, std::string* uncompressed) const;

    static std::string ErrorCode(int32_t error_code)
    {
        std::map<int32_t, std::string> sErrorCodeMap;
#define AddErrorCode(a) sErrorCodeMap[a] = #a
             AddErrorCode(Z_OK);
             AddErrorCode(Z_STREAM_END);
             AddErrorCode(Z_NEED_DICT);
             AddErrorCode(Z_ERRNO);
             AddErrorCode(Z_STREAM_ERROR);
             AddErrorCode(Z_DATA_ERROR);
             AddErrorCode(Z_MEM_ERROR);
             AddErrorCode(Z_BUF_ERROR);
             AddErrorCode(Z_VERSION_ERROR);
#undef AddErrorCode
        if (sErrorCodeMap.count(error_code) > 0u)
        {
            return  sErrorCodeMap.at(error_code);
        }
        else
        {
            return "Z_UNKNOWN";
        }
    }
};
REGISTER_COMPRESSOR("zlib", ZlibCompressor);

bool ZlibCompressor::Compress(const std::string& uncompressed, std::string* compressed) const
{
    if (compressed == NULL)
    {
        return false;
    }
    if (uncompressed.empty())
    {
        return true;
    }

    const char* src_data = uncompressed.c_str();
    size_t src_len = uncompressed.length();
    size_t buf_size = src_len;
    compressed->resize(buf_size);
    char* dest_data =  const_cast<char*>(compressed->c_str());

    uLongf compress_len = buf_size;
    uLongf real_src_len = src_len;

    int32_t level = 9;
    int32_t result =::compress2(reinterpret_cast<Bytef *>(dest_data),
                                &compress_len,
                                reinterpret_cast<const Bytef*>(src_data),
                                real_src_len,
                                level);
    if (result != Z_OK)
    {
        LOG(ERROR) << "zlib compress fail :"
                   << "uncompress length=[" << src_len  << "] "
                   << "compress length =[" << compress_len << "] "
                   << "error code=[" << ErrorCode(result) << "]";
        return false;
    }

    compressed->resize(compress_len);

    return true;
}

bool ZlibCompressor::Uncompress(const std::string& compressed, std::string* uncompressed) const
{
    if (uncompressed == NULL)
    {
        return false;
    }
    if (compressed.empty())
    {
        return true;
    }
    const char* src_data = compressed.c_str();
    size_t src_len = compressed.length();
    size_t buf_size =  FLAGS_default_uncompress_ratio * src_len;

    int result = Z_OK;
    uLongf len_uncompress = 0;
    for (int32_t i = 0; ; i++)
    {
        VLOG(0) << "i=[" << i << "] buffer size =[" << buf_size << "]";
        buf_size = std::min(buf_size, FLAGS_max_uncompress_buffer_size);
        uncompressed->resize(buf_size);
        // uLongf = uLong FAR = unsinged long = uint64_t in 64 bit
        len_uncompress = static_cast<uLongf>(buf_size);
        VLOG(0) << "buffer size =[" << len_uncompress << "]";
        char* dest_data =  const_cast<char*>(uncompressed->c_str());

        result =::uncompress(reinterpret_cast<Bytef *>(dest_data),
                             &len_uncompress,
                             reinterpret_cast<const Bytef *>(src_data),
                             src_len);
        if (result != Z_BUF_ERROR || buf_size >= FLAGS_max_uncompress_buffer_size)
        {
            break;
        }
        buf_size  *= 2;
    }
    if (result != Z_OK)
    {
        LOG(ERROR) << "zlib uncompress fail :"
                   << "uncompress length=[" << src_len  << "] "
                   << "compress length =[" << buf_size << "] "
                   << "error code=[" << ErrorCode(result) << "]";
        return false;
    }
    VLOG(0) << "final size =[" << len_uncompress << "]";
    uncompressed->resize(len_uncompress);
    return true;
}
