cc_binary(
    name = 'rtb5_server',
    srcs = [
        'rtb5_server.cc',
    ],
    deps = [
        '#cgicc',        
        '#grpc',
        '//thirdparty/glog:glog',
        '//business/rtb5/api:rtb5_service',
        '//common/base/string:string',
        '//common/system/concurrency:concurrency',
        '//thirdparty/jsoncpp:jsoncpp',
        '//thirdparty/protobuf:protobuf',
        '//thirdparty/stringencoders:stringencoders',
    ]
)