cc_library(
  name = 'writer',
  srcs = [
           'writer.cc',
         ],
  deps = [
          '//common/proto:config_pb',
          '//thirdparty/glog:glog',
          '//thirdparty/protobuf:protobuf',
          '//data_storer/sql:mysql_handler',
         ],
  allow_undefined = True,
)


cc_test(
    name = 'writer_test',
    srcs = [
               'writer_test.cc'
           ],
    deps = [
               ':writer',
               '//thirdparty/gtest:gtest',
           ],
    testdata =
           [
                ('testdata', 'testdata'),
           ],
)