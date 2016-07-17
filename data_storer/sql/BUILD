cc_library(
    name = 'mysql_handler',
    srcs = [
        'mysql_handler.cc',
    ],
    deps = [
        '//common/base/string:string',
        '//common/base/string:utility',
        '//thirdparty/jsoncpp:jsoncpp',
        '//thirdparty/protobuf:protobuf',
        '//thirdparty/mysql:mysqlclient',
        '//thirdparty/stringencoders:stringencoders',
        '//data_storer/sql/proto:mysql_config_pb',
        '//data_collector/proto:product_pb',
    ]
)

cc_binary(
  name = 'test',
  srcs = [
           'test.cc',
         ],
  deps = [
           ':mysql_handler',
           '//thirdparty/glog:glog',
           '//thirdparty/gflags:gflags',
         ],
)