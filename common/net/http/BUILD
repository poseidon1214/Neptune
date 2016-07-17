cc_library(
    name = "types",
    srcs = [
        "http_cookies.cc",
        "http_headers.cpp",
        "http_message.cpp",
        "http_request.cpp",
        "http_response.cpp",
        "http_time.cpp",
    ],
    deps = [
        "#z",
        "//common/base/string:string",
        "//common/collection:collection",
        "//common/net/mime:mime",
        "//thirdparty/snappy:snappy",
    ]
)

cc_library(
    name = "downloader",
    srcs = [
        "http_downloader.cpp",
    ],
    deps = [
        ":types",
        '//common/net/uri:uri',
        "//common/system/net:net",
        "//thirdparty/glog:glog"
    ]
)

cc_library(
    name = 'transfer',
    srcs = [
        'transfer.cc'
    ],
    deps = [
        "//thirdparty/glog:glog"
    ],
)

cc_library(
    name = 'http',
    deps = [
        ':types',
        ':downloader',
    ]
)
