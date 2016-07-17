proto_library(
    name = 'recordio_extent_header_proto',
    srcs = 'recordio_extent_header.proto'
)

cc_library(
    name = 'recordio',
    srcs = [
        'recordio.cc',
    ],
    deps = [
        ':recordio_extent_header_proto',
        '//thirdparty/glog:glog',
        '//thirdparty/protobuf:protobuf',
        '//app/qzap/common/compress:block_compress',
    ]
)

cc_library(
    name = 'recordio_jna',
    srcs = 'recordio_jna.cc',
    deps = ':recordio',
    link_all_symbols = True
)

cc_plugin(
    name = 'dynamic_recordio_jna',
    deps = ':recordio_jna'
)

proto_library(
    name = 'recordio_test_proto',
    srcs = 'recordio_test.proto'
)

cc_test(
    name = 'recordio_test',
    srcs = 'recordio_test.cc',
    deps = [
        ':recordio',
        '//app/qzap/common/utility:utility',
        ':recordio_test_proto',
    ],
    testdata = [
        ('testdata/master_file_list_old', 'master_file_list_old'),
        ('testdata/test_data.pb', 'test_data.pb'),
    ]
)

cc_test(
    name = 'recordio_jna_test',
    srcs = 'recordio_jna_test.cc',
    deps = ':recordio_jna'
)
