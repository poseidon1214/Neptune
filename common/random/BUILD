# Copyright (c) 2015 Tencent Inc.
# Author: Li Meng (elvisli@tencent.com)

cc_library(
    name = 'playback_random',
    srcs = 'playback_random.cc',
    deps = [
        '//app/qzap/common/utility:utility',
        '//common/system/concurrency:concurrency',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',
    ]
)

cc_test(
    name = 'playback_random_test',
    srcs = 'playback_random_test.cc',
    deps = ':playback_random',
)
