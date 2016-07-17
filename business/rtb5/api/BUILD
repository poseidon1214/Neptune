proto_library(
    name = 'rtb5_service_pb',
    srcs = 'rtb5_service.proto',
)

gen_rule(
    name='rtb5_service_grpc',
    cmd='protoc -I . --grpc_out=build64_release --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin business/rtb5/api/rtb5_service.proto',
    srcs = ['BUILD'],
    outs=['rtb5_service.grpc.pb.cc', 'rtb5_service.grpc.pb.h']
)

cc_library(
    name = 'rtb5_service',
    srcs = [
        'rtb5_service.grpc.pb.cc',
    ],
    deps = [
        ':rtb5_service_grpc',
        ':rtb5_service_pb',        
        '#grpc',
        '#grpc++_unsecure',
        '#gpr',
        '#protobuf',
    ]
)