cc_library(
    name = 'hash',
    srcs = [
        'md5.cpp',
        'sha1.cpp',
    ],
    deps = [
        '//thirdparty/openssl:crypto',
    ]
)

#cc_test(
#    name = 'md5_test',
#    srcs = 'md5_test.cpp',
#    deps = [
#        ':hash',
#        '//common/base/string:string'
#    ]
#)
#

cc_test(
    name = 'sha1_test',
    srcs = 'sha1_test.cpp',
    deps = [
        ':hash',
        '//common/base/string:string'
    ]
)
#
