# Copyright 2011, Tencnet Inc.
# Author: Xuemin Zhao (xueminzhao@tencent.com)
#         Yi Wang (yiwang@tencent.com)
#         Zhenlong Sun (richardsun@tencent.com)

proto_library(
    name = "lda_proto",
    srcs = "lda.proto")

cc_library(
    name = "cgo_types",
    srcs = "cgo_types.cc",
    deps = ["//thirdparty/glog:glog",])

cc_library(
    name = "city",
    srcs = "city.cc")

cc_test(
    name = "city_test",
    srcs = "city_test.cc",
    deps = [":city"])

cc_library(
    name = "common",
    srcs = "common.cc",
    deps = [":lda_proto",
            ":cgo_types",
            ":random",
            ":city"])

cc_library(
    name = "vocabulary",
    srcs = "vocabulary.cc",
    deps = ["//app/qzap/common/base:base",
            "//thirdparty/glog:glog",])

cc_test(
    name = "vocabulary_test",
    srcs = "vocabulary_test.cc",
    deps = [":vocabulary",],
    testdata = "testdata/vocab",)

cc_library(
    name = "dense_topic_histogram",
    srcs = "dense_topic_histogram.cc",
    deps = [":lda_proto",
            "//thirdparty/glog:glog",
            "//app/qzap/common/base:base",])

cc_test(
    name = "dense_topic_histogram_test",
    srcs = "dense_topic_histogram_test.cc",
    deps = [":dense_topic_histogram",])

cc_library(
    name = "ordered_sparse_histogram",
    srcs = "ordered_sparse_histogram.cc",
    deps = [":cgo_types",
            ":dense_topic_histogram",
            "//thirdparty/glog:glog",
            "//thirdparty/gtest:gtest",
            "//app/qzap/common/base:base",])

cc_test(
    name = "ordered_sparse_histogram_test",
    srcs = "ordered_sparse_histogram_test.cc",
    deps = [":ordered_sparse_histogram",
            ":dense_topic_histogram",],
    heap_check = 'local')

cc_library(
    name = "document",
    srcs = "document.cc",
    deps = [":ordered_sparse_histogram",
            ":common",
            ":model",
            ":vocabulary",
            ":random",
            "//thirdparty/glog:glog",])

cc_test(
    name = "document_test",
    srcs = "document_test.cc",
    deps = [":document",
            ":common",
            ":vocabulary",
            ":random",],
    testdata = ["testdata"],
    heap_check = 'local')

cc_library(
    name = "hyperparams",
    srcs = "hyperparams.cc",
    deps = [":lda_proto",
            "//app/qzap/text_analysis/topic/kv_records:kv_records",
            "//thirdparty/glog:glog"])

cc_test(
    name = "hyperparams_test",
    srcs = "hyperparams_test.cc",
    deps = [":hyperparams",])

cc_library(
    name = "global_stats",
    srcs = "global_stats.cc",
    deps = [":dense_topic_histogram",
            "//app/qzap/text_analysis/topic/kv_records:kv_records",
            "//thirdparty/glog:glog"])

cc_test(
    #dynamic_link=1,
    name = "global_stats_test",
    srcs = "global_stats_test.cc",
    deps = [":global_stats",])

cc_library(
    name = "word_stats",
    srcs = "word_stats.cc",
    deps = [":ordered_sparse_histogram",
            "//app/qzap/text_analysis/topic/kv_records:kv_records",
            "//app/qzap/common/base:base",
            "//thirdparty/glog:glog",
            "//app/qzap/text_analysis/topic/base:vocabulary",])

cc_test(
    #dynamic_link=1,
    name = "word_stats_test",
    srcs = "word_stats_test.cc",
    deps = [":word_stats",
            "//app/qzap/common/base:base",
            "//thirdparty/glog:glog",])

cc_library(
    name = "model",
    srcs = "model.cc",
    deps = [":hyperparams",
            ":common",
            ":global_stats",
            ":word_stats",
            "//thirdparty/glog:glog"])

cc_test(
    name = "model_test",
    srcs = "model_test.cc",
    deps = [":model"],
    testdata = "testdata")

cc_library(
    name = "random",
    srcs = "random.cc")

cc_test(
    name = "random_test",
    srcs = "random_test.cc",
    deps = [":random"])

cc_library(
    name = "smoothed_model_cache",
    srcs = "smoothed_model_cache.cc",
    deps = [":common",
            ":model",
            ":ordered_sparse_histogram",
            ":word_stats"])

cc_test(
    name = "smoothed_model_cache_test",
    srcs = "smoothed_model_cache_test.cc",
    deps = [":smoothed_model_cache",
            ":model"],
    testdata = "testdata")

