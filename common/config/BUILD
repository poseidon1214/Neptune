# Copyright (c) 2014 Tencent Inc.
# Author: Li Meng (elvisli@tencent.com)

cc_library(
    name = 'config',
    srcs = 'base_config_manager.cc',
    deps = [
        '//app/qzap/common/utility:utility',
        '//thirdparty/glog:glog',
        '//thirdparty/protobuf:protobuf',
    ],
)

proto_library(
    name = 'test_config_proto',
    srcs = 'test_config.proto',
)

cc_test(
    name = 'base_config_manager_test',
    srcs = 'base_config_manager_test.cc',
    deps = [
        ':config',
        ':test_config_proto',
    ],
    testdata = 'testdata/test_config.txt',
)

