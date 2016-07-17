cc_library(
    name = "kv_records",
    srcs = "kv_records.cc",
    deps = [
        "//thirdparty/glog:glog"
    ]
)

cc_test(
    name = "kv_records_test",
    srcs = "kv_records_test.cc",
    deps = [
        ":kv_records",
        "//thirdparty/glog:glog",
        "//thirdparty/gtest:gtest"
    ],
    testdata = [
        "testdata/records.kv"
    ]
)
