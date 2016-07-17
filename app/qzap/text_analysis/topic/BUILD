# Copyright 2013 Tencent Inc.
# Author: Lifeng Wang (fandywang@tencent.com)
# Description: the blade file for TopicInferenceEngine

cc_library(
    name = "topic_inference_engine",
    srcs = "topic_inference_engine.cc",
    deps = [
        "//app/qzap/text_analysis:text_miner_proto",
        "//app/qzap/text_analysis/topic/base:model",
        "//app/qzap/text_analysis/topic/base:vocabulary",
        "//app/qzap/text_analysis/topic/inference:multi_chains_gibbs_sampler",
        "//app/qzap/text_analysis/topic/inference:explainer",
        "//app/qzap/common/base:base",
        "//app/qzap/common/utility:utility",
    ]
)

cc_test(
    name = "topic_inference_engine_test",
    srcs = "topic_inference_engine_test.cc",
    deps = [
        ":topic_inference_engine",
        "//app/qzap/text_analysis:text_miner_proto",
    ],
    testdata = [
        "testdata/peacockmodel",
    ]
)

