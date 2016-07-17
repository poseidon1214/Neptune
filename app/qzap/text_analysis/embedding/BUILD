# Copyright 2015 Tencent Inc.
# Author: Lifeng Wang (fandywang@tencent.com)

# Description: the blade file for Embedding Inference Engine

cc_library(
  name = "embedding_inference_engine",
  srcs = "embedding_inference_engine.cc",
  deps = [
    "//app/qzap/text_analysis:dict_manager",
    "//app/qzap/text_analysis:text_miner_proto",
    "//app/qzap/common/base:base",
    "//app/qzap/common/utility:utility",
    "//thirdparty/gflags:gflags"
  ]
)

cc_test(
  name = "embedding_inference_engine_test",
  srcs = "embedding_inference_engine_test.cc",
  deps = [
    ":embedding_inference_engine",
    "//app/qzap/text_analysis:dict_manager",
    "//app/qzap/text_analysis:text_miner_proto",
    "//app/qzap/text_analysis/dict:word_embedding_dict",
  ],
  testdata = [
    ("//app/qzap/text_analysis/testdata/kedict", "./testdata/kedict")
  ]
)

