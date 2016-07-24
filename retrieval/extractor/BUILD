cc_binary(
  name = 'extractor_main',
  srcs = [
           'extractor_main.cc',
         ],
  deps = [
           '//common/config:config',
           '//common/writer:writer',
           '//common/reader:reader',
           '//data_collector/proto:feeder_config_pb',
           '//framework/proto:framework_pb',
           '//retrieval/proto:extract_pb',
           '//retrieval/proto:document_pb',
           '//thirdparty/glog:glog',
           '//thirdparty/gflags:gflags',
           '//thirdparty/gtest:gtest',
           '//thirdparty/protobuf:protobuf',
         ],
)