cc_library(
  name = 'timer',
  srcs = 'timer_manager.cc',
  deps = [
    '//common/system/time:time',
    '//common/system/concurrency:concurrency',
  ]
)

cc_test(
  name = 'timer_test',
  srcs = 'timer_manager_test.cc',
  deps = ':timer'
)
