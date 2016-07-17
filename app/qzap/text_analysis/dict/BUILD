# Copyright 2013 Tencent Inc.
# Author: Lifeng Wang (fandywang@tencent.com)
# Description:  the blade file for dictionary

cc_test(
    name = "dict_io_test",
    srcs = "dict_io_test.cc",
    deps = [
        ":dict_proto",
        "//thirdparty/glog:glog",
    ]
)

proto_library(
    name = "dict_proto",
    srcs = "dict.proto",
)

cc_library(
    name = "vocabulary",
    srcs = "vocabulary.cc",
    deps = [
        "//app/qzap/text_analysis/thirdparty:darts",
        "//app/qzap/common/base:base",
        "//thirdparty/glog:glog"
    ],
    link_all_symbols = 1
)

cc_test(
    name = "vocabulary_test",
    srcs = "vocabulary_test.cc",
    deps = [
        ":vocabulary"
    ],
    testdata = [
        "testdata/vocabulary",
    ]
)

cc_binary(
    name = "build_vocabulary",
    srcs = "build_vocabulary_main.cc",
    deps = [
        ":vocabulary",
    ]
)

cc_library(
    name = "stopword_dict",
    srcs = "stopword_dict.cc",
    deps = [
        ":vocabulary"
    ]
)

cc_test(
    name = "stopword_dict_test",
    srcs = "stopword_dict_test.cc",
    deps = [
        ":stopword_dict"
    ],
    testdata = [
        "testdata/dict.stopword"
    ]
)

cc_library(
    name = "strkey_value_dict_base",
    deps = [
        "//app/qzap/text_analysis/thirdparty:darts",
    ]
)

cc_test(
    name = "strkey_value_dict_base_test",
    srcs = "strkey_value_dict_base_test.cc",
    deps = [
        ":dict_proto",
        "//app/qzap/common/base:base",
        "//thirdparty/glog:glog",
        "//thirdparty/gtest:gtest",
        "//app/qzap/text_analysis/thirdparty:darts",
    ],
    testdata= [
        "testdata/keywords_test.dat",
    ]
)

cc_library(
    name = "keyword_dict",
    srcs = "keyword_dict.cc",
    deps = [
        ":dict_proto",
        "//app/qzap/common/base:base",
        "//app/qzap/text_analysis/thirdparty:darts",
        "//thirdparty/glog:glog"
    ],
    link_all_symbols = 1
)

cc_test(
    name = "keyword_dict_test",
    srcs = "keyword_dict_test.cc",
    deps = [
        ":dict_proto",
        ":keyword_dict",
        "//thirdparty/glog:glog",
        "//thirdparty/gtest:gtest"
    ],
    testdata = [
        "testdata/keyword.dat",
    ]
)

cc_binary(
    name = "build_keyword_dict",
    srcs = "build_keyword_dict_main.cc",
    deps = [
        ":keyword_dict",
    ]
)

cc_library(
    name = "token_idf_dict",
    srcs = "token_idf_dict.cc",
    deps = [
        ":dict_proto",
        "//app/qzap/text_analysis/thirdparty:darts",
        "//app/qzap/common/base:base",
        "//thirdparty/glog:glog"
    ],
    link_all_symbols = 1
)

cc_test(
    name = "token_idf_dict_test",
    srcs = "token_idf_dict_test.cc",
    deps = [
        ":token_idf_dict",
        "//thirdparty/gtest:gtest"
    ],
    testdata = [
        "testdata/idf",
    ]
)

cc_binary(
    name = "build_token_idf_dict",
    srcs = "build_token_idf_dict_main.cc",
    deps = [
        ":token_idf_dict",
    ]
)
"""
cc_library(
    name = "topic_word_dict",
    srcs = "topic_word_dict.cc",
    deps = [
        ":vocabulary"
    ]
)

cc_test(
    name = "topic_word_dict_test",
    srcs = "topic_word_dict_test.cc",
    deps = [
        ":topic_word_dict"
    ],
    testdata = [
        "testdata/dict.topic_word",
        "testdata/topic_word.dat",
    ]
)
"""

cc_library(
    name = "word_embedding_dict",
    srcs = "word_embedding_dict.cc",
    deps = [
        ":dict_proto",
        "//app/qzap/common/base:base",
        "//app/qzap/text_analysis/thirdparty:darts",
        "//thirdparty/glog:glog"
    ],
    link_all_symbols = 1
)

cc_test(
    name = "word_embedding_dict_test",
    srcs = "word_embedding_dict_test.cc",
    deps = [
        ":dict_proto",
        ":word_embedding_dict",
        "//thirdparty/glog:glog",
        "//thirdparty/gtest:gtest"
    ],
    testdata = [
        "testdata/embedding",
    ]
)

cc_binary(
    name = "build_word_embedding_dict_main",
    srcs = "build_word_embedding_dict_main.cc",
    deps = [
        ":word_embedding_dict",
    ]
)

