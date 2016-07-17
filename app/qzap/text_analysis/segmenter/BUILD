cc_library(
  name = "segmenter",
  srcs = "segmenter.cc",
  deps = [
    '//thirdparty/tcwordseg:TCWordSeg',
    '//common/system/concurrency:concurrency',
    '//thirdparty/gflags:gflags',
    '//thirdparty/glog:glog',
  ],
)

cc_test(
  name = "segmenter_test",
  srcs = "segmenter_test.cc",
  deps = ":segmenter",
  testdata = ('//thirdparty/tcwordseg/data', 'data'),
  heap_check = "local"
)
