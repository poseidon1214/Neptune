cc_library(
    name = 'check_error',
    srcs = 'check_error.cpp'
)

cc_library(
    name = 'info',
    srcs = 'system_information.cpp',
)

cc_library(
    name = 'path',
    srcs = 'path.cpp',
    deps = '//common/base/string:string',
)

cc_library(
  name = 'directory',
  srcs = 'directory.cpp',
  deps = [
    '//common/system:path',
    ':file',
  ]
)

cc_library(
  name = 'file',
  srcs = 'file.cpp',
)

cc_library(
  name = 'proc_stat',
  srcs = 'proc_stat.cpp',
  deps = '//thirdparty/glog:glog',
)

cc_test(
    name = 'path_test',
    srcs = 'path_test.cpp',
    deps = ':path',
)

cc_test(
  name = 'file_test',
  srcs = 'file_test.cpp',
  deps = ':file',
  testdata = ['test_unix.txt'],
)

cc_test(
  name = 'directory_test',
  srcs = 'directory_test.cpp',
  deps = ':directory',
)

cc_test(
  name = 'proc_stat_test',
  srcs = 'proc_stat_test.cpp',
  deps = ':proc_stat',
)
