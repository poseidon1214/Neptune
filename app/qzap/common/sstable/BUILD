cc_library(
  name = 'sstable',
  srcs = [
    'sstable.cc',
  ],
  deps = [
    '//thirdparty/leveldb:leveldb',
    '//app/qzap/common/utility:utility',
  ]
)

cc_library(
  name = 'hdfs_sstable',
  srcs = [
    'hdfs_sstable.cc',
  ],
  deps = [
    '//thirdparty/leveldb:leveldb',
    '//thirdparty/libhdfs:hdfs',
    '//app/qzap/common/utility:utility',
    '//app/qzap/common/base:base',
    ':sstable',
  ]
)

cc_test(
  name = 'sstable_test',
  srcs = [
    'sstable_test.cc',
  ],
  deps = [
    ':sstable',
  ],
)

cc_binary(
  name = 'hdfs_sstable_tester',
  srcs = [
    'hdfs_sstable_tester.cc',
  ],
  deps = [
    ':sstable',
    ':hdfs_sstable',
  ],
)
