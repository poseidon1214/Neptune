cc_library(
    name = 'binary_version',
    srcs = [
        'binary_version.cpp',
        'init_version_info.cpp',
    ],
    deps = '//thirdparty/gflags:gflags'
)

cc_test(
        name = 'binary_version_test',
        srcs = 'binary_version_test.cpp',
        deps = [
                    ':binary_version'
                ]
)
cc_library(
    name = 'callback',
    deps = '//thirdparty/protobuf:protobuf',
)

cc_library(
    name = 'export_variable',
    srcs = 'export_variable.cpp',
    deps = [
        '//common/system/concurrency:concurrency',
        '//thirdparty/jsoncpp:jsoncpp'
    ]
)

cc_test(
    name = 'export_variable_test',
    srcs = 'export_variable_test.cpp',
    deps = [
        ':export_variable'
    ]
)

cc_library(
    name = 'module',
    srcs = [
        'module/module.cpp',
        'module/dependency_map.cpp'
    ],
    deps = [
        '//thirdparty/glog:glog',
        '//thirdparty/gtest:gtest',
        ':binary_version'
    ]
)

cc_library(
    name = 'gflags_in_spp',
    srcs = 'gflags_in_spp.cc',
    deps = [
        '//thirdparty/gflags:gflags',
        '//common/base/string:string',
    ]
)

cc_library(
    name = 'static_resource',
    srcs = 'static_resource.cc',
    deps = '//common/base/string:string'
)

resource_library(
    name = 'static_resource_test_data',
    srcs = 'static_resource.testdata',
)

cc_test(
    name = 'static_resource_test',
    srcs = 'static_resource_test.cc',
    deps = [
        ':static_resource',
        ':static_resource_test_data'
    ]
)

cc_test(
    name = 'callback_test',
    srcs =
    ['callback_test.cc'],
    deps = [
        ':callback',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',
    ]
)

cc_test(
  name = 'class_register_test',
  srcs = [
    'class_register_test.cc',
    'class_register_test_helper.cc'
  ]
)

cc_test(
    name = 'scoped_ptr_test',
    srcs =
    ['scoped_ptr_test.cc'],
    deps = [
        '//thirdparty/glog:glog',
    ]
)

cc_test(
    name = 'shared_ptr_test',
    srcs =
    ['shared_ptr_test.cc'],
    deps = [
        ':callback',
        '//common/system/concurrency:concurrency',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',
    ]
)

cc_test(
    name = 'gflags_in_spp_test',
    srcs = 'gflags_in_spp_test.cc',
    deps = ':gflags_in_spp',
    testdata = [
        'test1.flags',
        'test2.flags',
    ]
)

cc_test(name = 'preprocess_test', srcs = 'preprocess_test.cc')
cc_test(name = 'singleton_test', srcs = 'singleton_test.cpp')
cc_test(name = 'static_assert_test', srcs = 'static_assert_test.cc')
cc_test(name = 'type_traits_test', srcs = 'type_traits_test.cpp')

cc_test(
  name = 'barrier_closure_test',
  srcs = 'barrier_closure_test.cc',
  deps = [
    '//app/qzap/common/base:base',
  ]
)

cc_test(
  name = 'scoped_mmap_test',
  srcs = 'scoped_mmap_test.cc',
)
