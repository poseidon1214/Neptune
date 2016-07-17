cc_library(
    name = 'encoding',
    srcs = [
        'base64.cpp',
        'charset_converter.cc',
        'html.cpp',
        'percent.cpp',
        'json_to_pb.cc',
        'cgi_to_pb.cc',
        'pb_to_map.cc',
        'pb_to_json.cc',
        'proto_converter.cc',
        'proto_json_format.cc',
        'shell.cpp',
        'compare_proto.cc',
        'xml_to_pb.cc',
    ],
    deps = [
        '#cgicc',
        '//common/base/string:string',
        '//common/proto:config_pb',
        '//common/system/concurrency:concurrency',
        '//thirdparty/glog:glog',
        '//thirdparty/jsoncpp:jsoncpp',
        '//thirdparty/libxml:xml2',
        '//thirdparty/protobuf:protobuf',
        '//thirdparty/stringencoders:stringencoders',
    ]
)

proto_library(
    name = 'pb_json_test_proto',
    srcs = 'pb_json_test.proto'
)

cc_test(
    name = 'base64_test',
    srcs = 'base64_test.cpp',
    deps = ':encoding'
)

cc_test(
    name = 'charset_converter_test',
    srcs = 'charset_converter_test.cc',
    deps = [
        ':encoding'
    ],
)

cc_test(
    name = 'html_test',
    srcs = 'html_test.cpp',
    deps = ':encoding',
)

cc_library(
  name = 'proto_comments',
  srcs = [
    'proto_file_extractor.cpp',
    'proto_comments_manager.cpp',
  ],
  deps = [
    '//common/base/string:string',
    '//common/system:file',
    '//thirdparty/protobuf:protobuf',
  ],
)

cc_test(
  name = 'proto_file_extractor_test',
  srcs = 'proto_file_extractor_test.cpp',
  deps = ':proto_comments',
)

cc_test(
  name = 'proto_comments_manager_test',
  srcs = 'proto_comments_manager_test.cpp',
  deps = [
    ':proto_comments',
    '//common/system:file',
  ]
)

cc_test(
  name = 'shell_test',
  srcs = 'shell_test.cpp',
  deps = ':encoding'
)


proto_library(
    name = 'find_field_test_proto',
    srcs = 'find_field_test.proto',
)

cc_test(
    name = 'compare_proto_test',
    srcs = 'compare_proto_test.cc',
    deps = [
        ':encoding',
        ':find_field_test_proto',
    ],
)
