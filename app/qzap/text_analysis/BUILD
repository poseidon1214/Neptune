# Copyright 2013 Tencent Inc.
# Author: Lifeng Wang (fandywang@tencent.com)
# Description: the blade file for TextMiner

cc_library(
    name = "dict_manager",
    srcs = "dict_manager.cc",
    deps = [
        "//app/qzap/text_analysis/dict:vocabulary",
        "//app/qzap/text_analysis/dict:keyword_dict",
        "//app/qzap/text_analysis/dict:stopword_dict",
        "//app/qzap/text_analysis/dict:token_idf_dict",
        "//app/qzap/text_analysis/dict:word_embedding_dict",
    ],
)

cc_test(
    name = "dict_manager_test",
    srcs = "dict_manager_test.cc",
    deps = [
        ":dict_manager",
        "//app/qzap/text_analysis/segmenter:segmenter",
        "//common/base/string:string",
        "//thirdparty/gtest:gtest",
        "//thirdparty/gflags:gflags"
    ],
    testdata = [
        ("//thirdparty/tcwordseg/data/", "testdata/tc_data"),
        "testdata/kedict",
    ],
)

proto_library(
    name = "text_miner_proto",
    srcs = "text_miner.proto",
)


cc_library(
    name = "text_miner_resource",
    srcs = "text_miner_resource.cc",
    deps = [
        ":dict_manager",
        "//app/qzap/text_analysis/token:token_extractor",
        "//app/qzap/text_analysis/keyword:keyword_extractor",
        "//app/qzap/text_analysis/topic:topic_inference_engine",
        "//app/qzap/text_analysis/classifier:classifier",
        "//app/qzap/text_analysis/embedding:embedding_inference_engine",
        "//app/qzap/text_analysis/segmenter:segmenter",
        "//app/qzap/common/thread:thread",
        "//app/qzap/common/utility:utility",
    ]
)

cc_test(
    name = "text_miner_resource_test",
    srcs = "text_miner_resource_test.cc",
    deps = [
        ":text_miner_resource",
        "//app/qzap/text_analysis/dict:stopword_dict",
        "//app/qzap/text_analysis/dict:keyword_dict",
        "//app/qzap/text_analysis/dict:token_idf_dict",
        "//app/qzap/text_analysis/dict:word_embedding_dict",
        "//thirdparty/gflags:gflags"
    ],
    testdata = [
        ("//thirdparty/tcwordseg/data/", "testdata/tc_data"),
        "testdata/text_miner_resource.config",
        "testdata/kedict",
        ("//app/qzap/text_analysis/topic/testdata/peacockmodel",
         "testdata/peacockmodel"),
        ("//app/qzap/text_analysis/classifier/testdata/classifier_model",
         "testdata/classifier_model"),
    ],
)

cc_library(
    name = "text_miner",
    srcs = "text_miner.cc",
    deps = [
        ":dict_manager",
        ":text_miner_proto",
        ":text_miner_resource",
        "//app/qzap/text_analysis/segmenter:segmenter",
        "//app/qzap/text_analysis/token:token_extractor",
        "//app/qzap/text_analysis/keyword:keyword_extractor",
        "//app/qzap/text_analysis/topic:topic_inference_engine",
        "//app/qzap/text_analysis/classifier:classifier",
        "//app/qzap/text_analysis/embedding:embedding_inference_engine",
    ],
    link_all_symbols = True
)

# generate dynamic lib
cc_plugin(
    name = "spa_text_miner",
    deps = [
        ":text_miner",
    ],
)

cc_test(
    name = "text_miner_test",
    srcs = "text_miner_test.cc",
    deps = [
        ":text_miner",
        "//app/qzap/common/thread:thread",
        "//app/qzap/text_analysis/dict:keyword_dict",
        "//app/qzap/text_analysis/dict:stopword_dict",
        "//app/qzap/text_analysis/dict:token_idf_dict",
        "//app/qzap/text_analysis/dict:word_embedding_dict",
        "//thirdparty/gflags:gflags"
    ],
    testdata = [
        ("//thirdparty/tcwordseg/data/",
         "testdata/tc_data"),
        "testdata/kedict",
        ("//app/qzap/text_analysis/topic/testdata/peacockmodel",
         "testdata/peacockmodel"),
        ("classifier/testdata/classifier_model", "testdata/classifier_model"),
        "testdata/text_miner_resource.config",
    ],
    heap_check = "local"
)

cc_library(
    name = "document_utils",
    srcs = "document_utils.cc",
    deps = [
        ":text_miner_proto",
        "//app/qzap/text_analysis/classifier:taxonomy_hierarchy",
    ],
)

cc_test(
    name = "document_utils_test",
    srcs = "document_utils_test.cc",
    deps = [
        ":document_utils",
    ],
    testdata = [
        "testdata/taxonomy"
    ]
)

cc_binary(
    name = "text_miner_main",
    srcs = "text_miner_main.cc",
    deps = [
        ":text_miner",
        "//common/base/string:string",
    ],
)

