# BUILD script, generated by make2build

cc_library(
    name = 'collection',
    deps = [
        '//common/system/concurrency:concurrency',
        '//common/crypto/hash:hash',
        '//common/base/string:string',
        '//thirdparty/glog:glog'
    ]
)

cc_test(
    name = 'prefix_map_test',
    srcs = [
        'prefix_map_test.cpp'
    ],
    deps = [
        ':collection',
    ]
)
