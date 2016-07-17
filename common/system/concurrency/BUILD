cc_library(
    name = 'this_thread',
    srcs = 'this_thread.cpp',
    deps = '#pthread'
)
#
cc_library(
    name = 'sync_object',
    srcs = [
        'condition_variable.cpp',
        'event.cpp',
        'spinlock.cpp',
    ],
    deps = [
        ':this_thread',
        '//common/system:check_error',
        '//common/system/time:posix_time',
        '#pthread',
    ]
)

cc_library(
    name = 'threadworker',
    srcs = 'threadworker.cc',
    deps = [
      ':sync_object',
      '//thirdparty/glog:glog',
    ],
)

cc_library(
    name = 'concurrency',
    srcs = [
        'base_thread.cpp',
        'thread.cpp',
        'thread_local.cpp',
        'thread_types.cpp',
    ],
    deps = [
        ':this_thread',
        ':sync_object',
        '//common/base/string:string',
        '//common/system:check_error',
        '//common/system:info',
        '//common/system/time:posix_time',
        '//thirdparty/protobuf:protobuf',
        '#pthread'
    ]
)

cc_test(
    name = 'blocking_queue_test',
    srcs = 'blocking_queue_test.cpp',
    deps = [
        ':concurrency',
        '//app/qzap/common/base:base',
    ]
)

#cc_test(
#    name = 'condition_variable_test',
#    srcs = 'condition_variable_test.cpp',
#    deps = [
#        ':concurrency'
#    ]
#)

cc_test(
    name = 'this_thread_test',
    srcs = 'this_thread_test.cpp',
    deps = [
        ':concurrency'
    ],
    heap_check = 'strict'
)

cc_test(
    name = 'thread_test',
    srcs = 'thread_test.cpp',
    deps = [
        ':concurrency'
    ],
    heap_check = 'strict'
)

cc_test(
    name = 'thread_local_test',
    srcs = 'thread_local_test.cpp',
    deps = ':concurrency',
    heap_check = 'strict'
)

#cc_test(
#    name = 'thread_pool_test',
#    srcs = 'thread_pool_test.cpp',
#    deps = [
#        ':concurrency',
#        '//thirdparty/perftools:profiler'
#    ]
#)
#
#cc_test(
#    name = 'fixed_thread_pool_test',
#    srcs = 'fixed_thread_pool_test.cpp',
#    deps = ':concurrency',
#)
#
#cc_test(
#    name = 'thread_group_test',
#    srcs = 'thread_group_test.cpp',
#    deps = ':concurrency'
#)
#
#cc_test(
#    name = 'semaphore_test',
#    srcs = 'semaphore_test.cpp',
#    deps = ':concurrency'
#)
#
cc_test(
    name = 'mutex_test',
    srcs = 'mutex_test.cpp',
    deps = ':concurrency',
    heap_check = 'local'
)

#cc_test(
#    name = 'spinlock_test',
#    srcs = 'spinlock_test.cpp',
#    deps = ':concurrency'
#)
#
#cc_test(
#    name = 'sequence_lock_test',
#    srcs = 'sequence_lock_test.cpp',
#    deps = [
#        ':sync_object'
#    ]
#)
#
#cc_test(
#    name = 'rwlock_test',
#    srcs = 'rwlock_test.cpp',
#    deps = ':concurrency'
#)
#
#cc_test(
#    name = 'typed_rwlock_test',
#    srcs = 'typed_rwlock_test.cpp',
#    deps = ':concurrency'
#)
#
cc_test(
    name = 'event_test',
    srcs = 'event_test.cpp',
    deps = [
        ':concurrency',
        '//thirdparty/glog:glog',
    ]
)

#cc_test(
#    name = 'linked_blocking_queue_test',
#    srcs = 'linked_blocking_queue_test.cpp',
#    deps = ':concurrency'
#)
#
#cc_test(
#    name = 'named_pipe_test',
#    srcs = 'named_pipe_test.cpp',
#    deps = ':concurrency'
#)
#
#cc_test(
#    name = 'once_test',
#    srcs = 'once_test.cpp',
#    deps = ':concurrency'
#)
