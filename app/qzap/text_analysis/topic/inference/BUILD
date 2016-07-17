# Copyright 2013, Tencnet Inc.
# Author: Xuemin Zhao (xueminzhao@tencent.com)
#         Yi Wang (yiwang@tencent.com)
#         Lifeng Wang (fandywang@tencent.com)

cc_library(
    name = "sparselda_gibbs_sampler",
    srcs = "sparselda_gibbs_sampler.cc",
    deps = ["//app/qzap/text_analysis/topic/base:model",
            "//app/qzap/text_analysis/topic/base:document",
            "//app/qzap/text_analysis/topic/base:global_stats",
            "//app/qzap/text_analysis/topic/base:word_stats",
            "//app/qzap/text_analysis/topic/base:random",
            "//app/qzap/text_analysis/topic/base:vocabulary",
            "//app/qzap/text_analysis/topic/base:smoothed_model_cache"])

cc_test(
    name = "sparselda_gibbs_sampler_test",
    srcs = "sparselda_gibbs_sampler_test.cc",
    deps = [":sparselda_gibbs_sampler",
            "//app/qzap/text_analysis/topic/base:model",
            "//app/qzap/text_analysis/topic/base:vocabulary",
            "//app/qzap/text_analysis/topic/base:common"],
    testdata = [("//app/qzap/text_analysis/topic/base/testdata",
                 "testdata")])

cc_library(
    name = "sparselda_hill_climber",
    srcs = "sparselda_hill_climber.cc",
    deps = ["//app/qzap/text_analysis/topic/base:model",
            "//app/qzap/text_analysis/topic/base:document",
            "//app/qzap/text_analysis/topic/base:global_stats",
            "//app/qzap/text_analysis/topic/base:word_stats",
            "//app/qzap/text_analysis/topic/base:random",
            "//app/qzap/text_analysis/topic/base:vocabulary",
            "//app/qzap/text_analysis/topic/base:smoothed_model_cache"])

cc_test(
    name = "sparselda_hill_climber_test",
    srcs = "sparselda_hill_climber_test.cc",
    deps = [":sparselda_hill_climber",
            "//app/qzap/text_analysis/topic/base:model",
            "//app/qzap/text_analysis/topic/base:vocabulary",
            "//app/qzap/text_analysis/topic/base:common"],
    testdata = [("//app/qzap/text_analysis/topic/base/testdata",
                 "testdata")])

cc_library(
    name = "multi_chains_gibbs_sampler",
    srcs = "multi_chains_gibbs_sampler.cc",
    deps = [":sparselda_gibbs_sampler",
            "//app/qzap/text_analysis/topic/base:common",
            "//app/qzap/text_analysis/topic/base:model",
            "//app/qzap/text_analysis/topic/base:document",
            "//app/qzap/text_analysis/topic/base:vocabulary"])

cc_test(
    name = "multi_chains_gibbs_sampler_test",
    srcs = "multi_chains_gibbs_sampler_test.cc",
    deps = [":multi_chains_gibbs_sampler",
            "//app/qzap/text_analysis/topic/base:model",
            "//app/qzap/text_analysis/topic/base:vocabulary",
            "//app/qzap/text_analysis/topic/base:common"],
    testdata = [("//app/qzap/text_analysis/topic/base/testdata",
                 "testdata")])

cc_library(
    name = "multi_trials_hill_climber",
    srcs = "multi_trials_hill_climber.cc",
    deps = [":sparselda_hill_climber",
            "//app/qzap/text_analysis/topic/base:common",
            "//app/qzap/text_analysis/topic/base:model",
            "//app/qzap/text_analysis/topic/base:document",
            "//app/qzap/text_analysis/topic/base:vocabulary",
            "//app/qzap/text_analysis/topic/base:model",])

cc_test(
    name = "multi_trials_hill_climber_test",
    srcs = "multi_trials_hill_climber_test.cc",
    deps = [":multi_trials_hill_climber",
            "//app/qzap/text_analysis/topic/base:model",
            "//app/qzap/text_analysis/topic/base:vocabulary",
            "//app/qzap/text_analysis/topic/base:common"],
    testdata = [("//app/qzap/text_analysis/topic/base/testdata",
                 "testdata")])

cc_library(
    name = "explainer",
    srcs = "explainer.cc",
    deps = ["//app/qzap/text_analysis/topic/base:common",
            "//app/qzap/text_analysis/topic/base:model",
            "//app/qzap/text_analysis/topic/base:vocabulary",
            "//app/qzap/text_analysis/topic/base:model",])

cc_test(
    name = "explainer_test",
    srcs = "explainer_test.cc",
    deps = [":explainer",
            ":sparselda_hill_climber",
            ":sparselda_hill_climber",
            ":multi_chains_gibbs_sampler",
            ":multi_trials_hill_climber",
            "//app/qzap/text_analysis/topic/base:common",
            "//app/qzap/text_analysis/topic/base:model",
            "//app/qzap/text_analysis/topic/base:vocabulary",
            "//app/qzap/text_analysis/topic/base:model"],
    testdata = [("//app/qzap/text_analysis/topic/base/testdata",
                 "testdata")])

