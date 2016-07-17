cc_library(
  name = 'rules_common',
  srcs = [
           'rules_common.cc',
         ],
  deps = [
          '//common/proto:config_pb',
          '//common/encoding:encoding',
          '//app/qzap/common/utility:utility',
          '//thirdparty/protobuf:protobuf',
         ],
  allow_undefined = True,
)

cc_test(
    name = 'rules_common_test',
    srcs = 'rules_common_test.cc',
    deps = [
        ':rules_common',
        '//thirdparty/glog:glog',
    ]
)
