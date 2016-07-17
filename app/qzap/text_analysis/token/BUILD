# Copyright 2013 Tencent Inc.
# Author: Lifeng Wang (fandywang@tencent.com)

# Description: the blade file for Token Extractor

cc_library(
  name = "token_extractor",
  srcs = "token_extractor.cc",
  deps = [
    "//app/qzap/text_analysis:dict_manager",
    "//app/qzap/text_analysis:text_miner_proto",
    "//app/qzap/common/base:base",
    "//app/qzap/common/utility:utility",
    "//thirdparty/gflags:gflags"
  ]
)

cc_test(
  name = "token_extractor_test",
  srcs = "token_extractor_test.cc",
  deps = [
    ":token_extractor",
    "//app/qzap/text_analysis:dict_manager",
    "//app/qzap/text_analysis:text_miner_proto",
    "//app/qzap/text_analysis/dict:token_idf_dict",
  ],
  testdata = [
    ("//app/qzap/text_analysis/testdata/kedict", "./testdata/kedict")
  ]
)

