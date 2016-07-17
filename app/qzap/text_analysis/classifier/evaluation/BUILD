cc_library(
    name = "classifier_evaluator",
    srcs = "classifier_evaluator.cc",
    deps = [
        "//app/qzap/common/base:base",
        "//app/qzap/text_analysis:text_miner",
        "//app/qzap/text_analysis/classifier/feature:feature_extractor",
        "//app/qzap/text_analysis/classifier:hierarchical_classifier",
        "//app/qzap/text_analysis/classifier:instance",
    ]
)

cc_binary(
    name = "classifier_evaluator_main",
    srcs = "classifier_evaluator_main.cc",
    deps = [
        ":classifier_evaluator",
    ]
)

cc_binary(
    name = "print_string_features_main",
    srcs = "print_string_features_main.cc",
    deps = [
        ":classifier_evaluator",
        "//app/qzap/common/base:base",
    ]
)
