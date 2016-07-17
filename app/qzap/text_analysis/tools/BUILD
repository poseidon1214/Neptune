# Copyright 2014 Tencent Inc.
# Author: Lifeng Wang (fandywang@tencent.com)
# Description: the blade file for TextMiner

cc_binary(
    name = "text_segmenter_main",
    srcs = "text_segmenter_main.cc",
    deps = [
        "//thirdparty/gflags:gflags",
        "//thirdparty/glog:glog",
        "//app/qzap/common/base:string",
        "//app/qzap/text_analysis/segmenter:segmenter",
    ],
)

cc_binary(
    name = "topic_dumper_main",
    srcs = "topic_dumper_main.cc",
    deps = [
        "//app/qzap/text_analysis:text_miner",
        "//app/qzap/text_analysis:text_miner_proto",
        "//common/base/string:string",
        "//thirdparty/gflags:gflags"
    ]
)

cc_binary(
    name = "token_dumper_main",
    srcs = "token_dumper_main.cc",
    deps = [
        "//app/qzap/text_analysis:text_miner",
        "//app/qzap/text_analysis:text_miner_proto",
        "//common/base/string:string",
        "//thirdparty/gflags:gflags"
    ]
)

cc_binary(
    name = "keyword_dumper_main",
    srcs = "keyword_dumper_main.cc",
    deps = [
        "//app/qzap/text_analysis:text_miner",
        "//app/qzap/text_analysis:text_miner_proto",
        "//common/base/string:string",
        "//thirdparty/gflags:gflags"
    ]
)



cc_binary(
    name = "embedding_distance",
    srcs = "embedding_distance.cc",
    deps = [
        "//thirdparty/gflags:gflags",
    ],
)

cc_binary(
    name = "text_miner_feature_dumper",
    srcs = "text_miner_feature_dumper.cc",
    deps = [
        "//thirdparty/gflags:gflags",
        "//thirdparty/glog:glog",
        "//app/qzap/common/base:string",
        "//app/qzap/text_analysis:text_miner",
    ],
)
