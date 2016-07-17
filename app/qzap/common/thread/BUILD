cc_library(
  name = 'thread',
  srcs =
   ['mutex.cc',
    'shared_mutex.cc',
    'threadworker.cc',
   ],
  deps = [
   '//app/qzap/common/base:base',
   '//common/system/concurrency:sync_object',
   '//common/system/concurrency:threadworker',
   '#pthread'
  ]
)

cc_test(
  name = 'mutex_test',
  srcs =
    [ 'mutex_test.cc'],
  deps = [
   ':thread',
  ]
)

cc_test(
  name = 'shared_mutex_test',
  srcs =
    [ 'shared_mutex_test.cc'],
  deps = [
   ':thread',
  ]
)

cc_test(
  name = 'threadpool_test',
  srcs =
    [ 'threadpool_test.cc'],
  deps = [
   ':thread',
   '//app/qzap/common/base:base',
   '//app/qzap/common/utility:utility',
  ]
)

cc_test(
  name = 'thread_safe_table_test',
  srcs =
    [ 'thread_safe_table_test.cc'],
  deps = [
   ':thread',
   '//app/qzap/common/utility:utility',
  ]
)
