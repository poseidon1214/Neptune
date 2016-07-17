cc_library(
    name = 'compress',
    srcs = [
        'compressor.cc',
    ],
    deps = [
#        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',
        '//thirdparty/zlib:z',
    ],
#    link_all_symbols = True,
)

cc_test(
    name = 'compress_test',
    srcs = [
        'compressor_test.cc',
    ],
    deps =  [
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',
        '//app/qzap/common/utility:utility',
        ':compress',
    ],
)

cc_library(
    name = 'block_compress',
    srcs = [
        'bmz/bmdiff.cpp',
        'bmz/bmz.cpp',
        'lzo/minilzo.c',
        'quicklz/quicklz.cpp',
        'block_compression_codec_bmz.cpp',
        'block_compression_codec.cpp',
        'block_compression_codec_lzo.cpp',
        'block_compression_codec_manager.cpp',
        'block_compression_codec_none.cpp',
        'block_compression_codec_quicklz.cpp',
        'block_compression_codec_snappy.cpp',
        'block_compression_codec_zlib.cpp',
        'block_compression_codec_gzip.cpp',
    ],
    deps = [
        '//thirdparty/snappy:snappy',
        '//common/base/compatible:compatible',
        '//common/system/concurrency:concurrency',
        '#z',
        './checksum:checksum'
    ]
)

cc_test(
    name = 'block_compression_codec_test',
    srcs = 'block_compression_codec_test.cpp',
    deps = [
           ':block_compress',
           '//common/base/string:string',
           ],
    testdata = 'data'
)

