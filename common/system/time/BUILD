cc_library(
    name = 'posix_time',
    srcs = 'posix_time.cpp',
)

cc_library(
    name = 'time',
    srcs = [
        'clock.cc',
    ],
    deps = [
        ':posix_time',
        '#rt',
    ]
)

cc_test(
    name = 'clock_test',
    srcs = 'clock_test.cc',
    deps = ':time',
)

#cc_test(
#    name = 'datetime_test',
#    srcs = 'datetime_test.cpp',
#    deps = [
#        ':time',
#        '//common/base/string:string',
#        '//thirdparty/glog:glog',
#    ]
#)
#
#cc_test(
#    name = 'stopwatch_test',
#    srcs = 'stopwatch_test.cpp',
#    deps = [
#        ':time',
#        '//common/system/concurrency:concurrency'
#    ]
#)
#
#cc_test(
#    name = 'timestamp_test',
#    srcs = 'timestamp_test.cpp',
#    deps = [
#        '#rt',
#        ':time',
#        '//common/system/concurrency:concurrency'
#    ]
#)
#
#cc_test(
#    name = 'time_t_test',
#    srcs = 'time_t_test.cpp',
#    deps = [
#        ':time',
#    ]
#)
#
#cc_test(
#    name = 'time_utils_test',
#    srcs = 'time_utils_test.cpp',
#    deps = [
#        ':time',
#        '//thirdparty/glog:glog',
#    ]
#)
#
#cc_binary(
#    name = 'test_tsc',
#    srcs = 'test_tsc.cpp',
#    deps = [
#        ':time',
#        '//common/system/concurrency:concurrency'
#    ]
#)
