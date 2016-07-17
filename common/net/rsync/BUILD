cc_library(
  name = 'rsync',
  srcs = [
    'rsync.cpp',
  ],
  deps = [
    '//common/base/string:string',
    '//common/encoding:encoding',
  ],
)

cc_test(
  name = 'rsync_test',
  srcs = [
    'rsync_test.cpp',
  ],
  deps = [
    '//common/net/rsync:rsync',
    '//app/qzap/common/utility:utility',
  ]
)
