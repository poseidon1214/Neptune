cc_binary(
  name = 'extractor_main',
  srcs = [
           'extractor_main.cc',
         ],
  deps = [
           '//app/qzap/text_analysis:text_miner',
           '//common/config:config',
           '//common/base/string:string',
           '//common/writer:writer',
           '//common/reader:reader',
           '//data_collector/proto:feeder_config_pb',
           '//data_collector/feeder/common:rules_common',
           '//framework/proto:framework_pb',
           '//retrieval/proto:extract_pb',
           '//retrieval/proto:document_pb',
           '//thirdparty/glog:glog',
           '//thirdparty/gflags:gflags',
           '//thirdparty/gtest:gtest',
           '//thirdparty/protobuf:protobuf',
         ],
)