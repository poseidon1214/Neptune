# Copyright (c) 2014, Tencent Inc
# Author: Yu Xie <yuxie@tencent.com>

cc_library(
    name = 'base_task',
    srcs = 'base_task.cc',
    deps = [
        '//common/base:callback',
        '//thirdparty/glog:glog',
    ],
)

cc_library(
    name = 'state_machine',
    srcs = 'state_machine.cc',
    deps = [
        ':base_task',
        '//app/qzap/common/base:base',
        '//common/base:callback',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',
    ],
)

cc_test(
    name = 'state_machine_test',
    srcs = 'state_machine_test.cc',
    deps = [
        ':state_machine',
        '//common/base:callback',
        '//common/system/concurrency:sync_object',
        '//thirdparty/glog:glog',
        '//thirdparty/gmock:gmock',
        '//thirdparty/gtest:gtest',
    ],
)
