cc_library(
  name = 'bit_array',
  srcs = ['bit_array.cc'],
)

cc_library(
  name = 'wat_array',
  srcs = ['wat_array.cc'],
  deps = [
           ':bit_array',
         ]
)

cc_library(
  name = 'wavelet_matrix',
  srcs = ['wavelet_matrix.cc'],
  deps = [
           ':wat_array',
         ]
)

cc_library(
  name = 'wavelet_tree',
  srcs = ['wavelet_tree.cc'],
  deps = [
           '//thirdparty/glog:glog',
           '//thirdparty/gflags:gflags',
           ':wavelet_matrix',
         ],
)

cc_test(
    name = 'wat_array_test',
    srcs = [
               'wat_array_test.cc'
           ],
    deps = [
               ':wat_array',
               '//thirdparty/gtest:gtest',
           ],
)

cc_test(
    name = 'wavelet_matrix_test',
    srcs = [
               'wavelet_matrix_test.cc'
           ],
    deps = [
               ':wavelet_matrix',
               '//thirdparty/gtest:gtest',
           ],
)

cc_test(
    name = 'wavelet_tree_test',
    srcs = [
               'wavelet_tree_test.cc'
           ],
    deps = [
               ':wavelet_tree',
               '//thirdparty/gtest:gtest',
           ],
)

cc_library(
  name = 'similarity_retrievaler',
  srcs = ['similarity_retrievaler.cc'],
  deps = [
           '//thirdparty/glog:glog',
           '//thirdparty/gflags:gflags',
           '//app/qzap/common/utility:utility',
           '//common/base/string:string',
           ':wavelet_tree',
         ],
  allow_undefined = True,
)