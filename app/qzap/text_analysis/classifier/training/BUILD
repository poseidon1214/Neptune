
cc_library(
  name = "train_classifier_base",
  srcs = "train_classifier_base.cc",
  deps = [
      "//app/qzap/common/base:base",
    "//app/qzap/text_analysis/classifier:instance",
    "//app/qzap/text_analysis/classifier:taxonomy_hierarchy",
  ]
)

cc_test(
  name = "train_classifier_base_test",
  srcs = "train_classifier_base_test.cc",
  deps = [
    ":train_classifier_base",
  ],
)

cc_library(
  name = "train_maxent",
  srcs = "train_maxent.cc",
  deps = [
    ":train_classifier_base",
    "//app/qzap/text_analysis/thirdparty/maxent:maxent",
  ]
)

cc_test(
  name = "train_maxent_test",
  srcs = "train_maxent_test.cc",
  deps = [
    ":train_maxent",
  ],
  testdata = [
    "testdata/taxonomy",
    "testdata/training_samples",
  ]
)

cc_library(
  name = "train_hierarchical_classifier",
  srcs = "train_hierarchical_classifier.cc",
  deps = [
    ":train_maxent",
  ]
)

cc_test(
  name = "train_hierarchical_classifier_test",
  srcs = "train_hierarchical_classifier_test.cc",
  deps = [
    ":train_hierarchical_classifier",
  ],
  testdata = [
    "testdata/taxonomy",
    "testdata/training_samples",
  ],
)

cc_binary(
  name = "hierarchical_classifier_trainer",
  srcs = "hierarchical_classifier_trainer_main.cc",
  deps = [
    ":train_hierarchical_classifier",
  ]
)
