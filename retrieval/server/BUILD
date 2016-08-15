cc_binary(
    name = 'retrieval_server',
    srcs = [
        'retrieval_server.cc',
    ],
    deps = [
        '#cgicc',        
        '#grpc',
        '//common/base/string:string',
        '//common/system/concurrency:concurrency',
        '//retrieval/proto:retrieval_service',
        '//thirdparty/glog:glog',
        '//thirdparty/jsoncpp:jsoncpp',
        '//thirdparty/protobuf:protobuf',
        '//thirdparty/stringencoders:stringencoders',
        '//retrieval/fuzzy/wavelet:similarity_retrievaler',
    ]
)