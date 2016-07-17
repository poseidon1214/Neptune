cc_library(
    name = "time",
    srcs = "time.cc",
    deps = ["//thirdparty/glog:glog",
            "//app/qzap/common/utility:utility"])

cc_test(
    name = "time_test",
    srcs = "time_test.cc",
    deps = ":time",)

cc_library (
  name = 'classifier',
  srcs = 'classifier.cc',
  deps = [
    '//thirdparty/glog:glog',
    '//app/qzap/text_analysis/classifier:instance',
    '//app/qzap/text_analysis/classifier:hierarchical_classifier',
    '//app/qzap/text_analysis/dict:vocabulary',
    '//app/qzap/text_analysis:text_miner_proto',
    '//app/qzap/text_analysis/classifier/feature:feature_extractor',
  ],
)

cc_test (
  name = 'classifier_test',
  srcs = 'classifier_test.cc',
  deps = [
    ':classifier',
    '//thirdparty/gflags:gflags',
  ],
  testdata = [
    'testdata/classifier_model',
  ],
)

proto_library(
    name = 'classifier_proto',
    srcs = 'classifier.proto',
)

cc_library(
    name = 'instance',
    srcs = 'instance.cc',
    deps = [
        '//thirdparty/glog:glog',
        '//app/qzap/common/base:base',
    ]
)

cc_test(
    name = 'instance_test',
    srcs = 'instance_test.cc',
    deps = ':instance',
)

cc_library(
    name = "taxonomy_hierarchy",
    srcs = "taxonomy_hierarchy.cc",
    deps = ["//thirdparty/glog:glog",
            "//app/qzap/common/base:base",]
)

cc_test(
    name = "taxonomy_hierarchy_test",
    srcs = "taxonomy_hierarchy_test.cc",
    deps = [":taxonomy_hierarchy"],
    testdata = "testdata/test_taxonomy_hierarchy"
)

cc_library(
    name = "maxent",
    srcs = "maxent.cc",
    deps = [":taxonomy_hierarchy",
            ":instance",
            ":classifier_proto",
            "//app/qzap/common/base:base",
            "//thirdparty/glog:glog",
            "//app/qzap/text_analysis/thirdparty/maxent:maxent",]
)

cc_test(
    name = "maxent_test",
    srcs = "maxent_test.cc",
    deps = [":maxent",
            "//app/qzap/common/base:base"],
    testdata = ["testdata/test_hierarchical_classifier_model",
                "testdata/test_classifier_instances"],
)

cc_library(
    name = "optim_maxent",
    srcs = "optim_maxent.cc",
    deps = [":taxonomy_hierarchy",
            ":instance",
            ":classifier_proto",
            "//thirdparty/glog:glog",
            "//app/qzap/common/base:base",]
)

cc_test(
    name = "optim_maxent_test",
    srcs = "optim_maxent_test.cc",
    deps = [":optim_maxent",
            ":maxent",
            "//app/qzap/text_analysis/classifier:time"],
    testdata = ["testdata/test_hierarchical_classifier_model/11",
                "testdata/test_classifier_instances"]
)

cc_library(
    name = "hierarchical_classifier",
    srcs = "hierarchical_classifier.cc",
    deps = [":optim_maxent",
            ":classifier_proto"]
)

cc_test(
    name = "hierarchical_classifier_test",
    srcs = "hierarchical_classifier_test.cc",
    deps = [":hierarchical_classifier",
            "//app/qzap/common/base:base"],
    testdata = ["testdata/test_hierarchical_classifier_model",
                "testdata/test_classifier_instances"],
)

