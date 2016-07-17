// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>
#include "app/qzap/common/compress/compressor.h"

#include <string>

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/utility/file_utility.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/gtest/gtest.h"

using std::string;
DECLARE_int32(default_uncompress_ratio);
TEST(CompressorTest, zlib_size_1k)
{
    scoped_ptr<Compressor> compressor(Compressor::GetCompressor("zlib"));
    ASSERT_NE(compressor.get(), reinterpret_cast<Compressor*>(NULL));
    string original(1024, 'a');
    string compress;
    ASSERT_TRUE(compressor->Compress(original, &compress));
    string uncompress;
    ASSERT_TRUE(compressor->Uncompress(compress, &uncompress));
    ASSERT_EQ(original, uncompress);
}
TEST(CompressorTest, zlib_size_1M_and_large_default_uncompress_ratio)
{
    scoped_ptr<Compressor> compressor(Compressor::GetCompressor("zlib"));
    ASSERT_NE(compressor.get(), reinterpret_cast<Compressor*>(NULL));
    string original(1024 * 1024, 'a');
    string compress;
    ASSERT_TRUE(compressor->Compress(original, &compress));
    string uncompress;
    FLAGS_default_uncompress_ratio = 100;
    ASSERT_TRUE(compressor->Uncompress(compress, &uncompress));
    ASSERT_EQ(original, uncompress);
}
TEST(CompressorTest, zlib_size_1G)
{
    scoped_ptr<Compressor> compressor(Compressor::GetCompressor("zlib"));
    ASSERT_NE(compressor.get(), reinterpret_cast<Compressor*>(NULL));

    string original(1ull << 30, 'a');
    string compress;
    ASSERT_TRUE(compressor->Compress(original, &compress));
    ASSERT_TRUE(WriteStringToFile("zlib_size_1g.dat", compress));

}
TEST(CompressorTest, zlib_size_1G_uncompress)
{
    scoped_ptr<Compressor> compressor(Compressor::GetCompressor("zlib"));
    ASSERT_NE(compressor.get(), reinterpret_cast<Compressor*>(NULL));

    string compress;
    ASSERT_TRUE(ReadFileToString("zlib_size_1g.dat", &compress));
    string uncompress;
    ASSERT_TRUE(compressor->Uncompress(compress, &uncompress));
    ASSERT_EQ(uncompress.size(), 1ull << 30);
    ASSERT_EQ(uncompress[0], 'a');
}
