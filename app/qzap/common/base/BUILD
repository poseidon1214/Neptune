cc_library(
  name = 'string',
  srcs = 'string_utility.cc',
  deps = [
   '//common/base/string:string',
   '//common/base/string:utility',
  ]
)

cc_library(
  name = 'base',
  srcs =
   ['walltime.cc',
    'sysinfo.cc',
   ],
  deps = [
   ':string',
   '#pthread',
   '#rt',
   '//common/base:callback',
   '//thirdparty/glog:glog',
   '//thirdparty/gflags:gflags',
   '//thirdparty/openssl:ssl',
   '//common/system/concurrency:concurrency',
  ]
)

cc_library(
  name = 'benchmark',
  srcs =
   [
    'benchmark.cc',
   ],
  deps = [
   ':base',
   '//app/qzap/common/thread:thread',
  ]
)

cc_test(
  name = 'string_utility_test',
  srcs =
   ['string_utility_test.cc'],
  deps = [
   '//thirdparty/gflags:gflags',
   '//thirdparty/glog:glog',
   ':base',
  ]
)

cc_test(
  name = 'thread_test',
  srcs =
   ['thread_test.cc'],
  deps = [
   '//thirdparty/gflags:gflags',
   '//thirdparty/glog:glog',
   ':base',
  ]
)

cc_test(
  name = 'walltime_test',
  srcs =
   ['walltime_test.cc'],
  deps = [
   ':base',
   '//thirdparty/glog:glog',
  ]
)

cc_library(
  name = 'binary_version',
  deps = '//common/base:binary_version',
  deprecated = True,
)

