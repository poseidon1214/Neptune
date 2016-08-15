proto_library(
  name = 'document_pb',
  srcs = ['document.proto'],
)

proto_library(
  name = 'extract_pb',
  srcs = ['extract.proto'],
  deps = [
    '//common/proto:config_pb',
  ]
)

proto_library(
  name = 'query_pb',
  srcs = ['query.proto'],
)

proto_library(
    name = 'retrieval_service_pb',
    srcs = 'retrieval_service.proto',
)

gen_rule(
    name='retrieval_service_grpc',
    cmd='protoc -I . --grpc_out=build64_release --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin retrieval/proto/retrieval_service.proto',
    srcs = ['BUILD'],
    outs=['retrieval_service.grpc.pb.cc', 'retrieval_service.grpc.pb.h']
)

cc_library(
    name = 'retrieval_service',
    srcs = [
        'retrieval_service.grpc.pb.cc',
    ],
    deps = [
        ':retrieval_service_grpc',
        ':retrieval_service_pb',        
        '#grpc',
        '#grpc++_unsecure',
        '#gpr',
        '#protobuf',
    ]
)