# Copyright 2013 Tencent Inc.
# Author: Lifeng Wang (fandywang@tencent.com)
# Description: the blade file for Keyword Extractor

cc_library(
    name= "keyword_extractor",
    srcs= "keyword_extractor.cc",
    deps= [
        "//app/qzap/text_analysis/dict:dict_proto",
        "//app/qzap/text_analysis:dict_manager",
        "//app/qzap/text_analysis:text_miner_proto",
        "//app/qzap/text_analysis/dict:dict_proto",
        "//app/qzap/text_analysis/dict:keyword_dict",
        "//app/qzap/text_analysis/dict:stopword_dict",
        "//app/qzap/text_analysis/dict:vocabulary",
        "//app/qzap/common/base:base",
        "//app/qzap/common/utility:utility",
        "//thirdparty/gflags:gflags",
        "//thirdparty/glog:glog",
    ]
)

cc_test(
    name= "keyword_extractor_test",
    srcs= "keyword_extractor_test.cc",
    deps= [
        ":keyword_extractor",
        "//app/qzap/text_analysis:dict_manager",
        "//thirdparty/glog:glog",
    ],
    testdata= [
        ("//thirdparty/tcwordseg/data", "./testdata/tc_data"),
        ("//app/qzap/text_analysis/testdata/kedict", "./testdata/kedict")
    ]
)

