
cc_library(
  name = 'zk_leader_election',
  srcs = 'zk_leader_election.cpp',
  deps = [
    '//common/base/string:string',
    '//common/system/concurrency:concurrency',
    '//thirdparty/zookeeper:zookeeper_mt',
    '//thirdparty/glog:glog'
  ]
)

cc_library(
  name = 'zk_adaptor',
  srcs = 'zk_adaptor.cpp',
  deps = [
    '//common/encoding:encoding',
    '//common/system/concurrency:concurrency',
    '//thirdparty/glog:glog',
    '//thirdparty/gflags:gflags',
    '//thirdparty/zookeeper:zookeeper_mt',
  ],
)

cc_test(
  name = 'zk_adaptor_test',
  srcs = 'zk_adaptor_test.cpp',
  deps = [
    ':zk_adaptor',
  ]
)

