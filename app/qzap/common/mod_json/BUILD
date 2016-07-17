# created by yunfengxiao

cc_library(
    name = 'mod_json',
    srcs = [
        'src/mod_hashtab.c',
        'src/mod_json.c',
        'src/mod_json.cc',
    ],
)

cc_test(
    name = 'mod_json_test',
    srcs = 'src/mod_json_test.cc',
    deps = [
        ':mod_json'
    ]
)

