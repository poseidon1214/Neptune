# Copyright (C), 1998-2012, Tencent
# Author: jefftang@tencent.com
# Date: 2012-11-22

cc_library(
  name = 'utility',
  srcs = [
    'file_utility.cc',
    'net_utility.cc',
    'lookup8.c',
    'time_utility.cc',
    'time_utility_internal.cc',
    'uuid_utility.cc',
    'mod_setaffinity.cc',
    'true_random.cc',
  ],
  deps = [
    '//app/qzap/common/base:base',
    '//thirdparty/uuid:uuid',
  ]
)

cc_test(
    name = 'true_random_test',
    srcs = 'true_random_test.cc',
    deps = ':utility'
)

