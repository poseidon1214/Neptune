cc_library(
    name = "feature_extractor",
    srcs = "feature_extractor.cc",
    deps = [
        "//thirdparty/glog:glog",
        "//app/qzap/common/base:base",
        "//app/qzap/text_analysis:text_miner_proto",
        "//app/qzap/text_analysis/dict:vocabulary",
        "//app/qzap/text_analysis/classifier:instance",
    ]
)



cc_binary(
    name = "gen_feature_vocabulary",
    srcs = "gen_feature_vocabulary_main.cc",
    deps = [
        "//app/qzap/common/base:base",
        "//app/qzap/text_analysis:text_miner",
    ]
)

cc_binary(
    name = "feature_extractor_main",
    srcs = "feature_extractor_main.cc",
    deps = [
        ":feature_extractor",
        "//app/qzap/text_analysis:text_miner",
        "//app/qzap/text_analysis/classifier:instance",
    ]
)

