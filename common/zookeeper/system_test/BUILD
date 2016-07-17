
cc_binary(
  name = 'select_leader_async',
  srcs = 'zk_leader_election_async.cpp',
  deps = [
    '//common/base:callback',
    '//common/system/concurrency:this_thread',
    '//common/zookeeper:zk_leader_election',
    '//thirdparty/gflags:gflags',
    '//thirdparty/glog:glog',
  ],
)

cc_binary(
  name = 'select_leader_sync',
  srcs = 'zk_leader_election_sync.cpp',
  deps = [
    '//common/base:callback',
    '//common/system/concurrency:this_thread',
    '//common/zookeeper:zk_leader_election',
    '//thirdparty/gflags:gflags',
    '//thirdparty/glog:glog',
  ],
)

