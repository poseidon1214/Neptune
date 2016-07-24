cc_library(
  name = 'reader',
  srcs = [
           'reader.cc',
         ],
  deps = [
          '//common/proto:config_pb',
          '//common/encoding:encoding',
          '//data_storer/sql:mysql_handler',
          '//thirdparty/protobuf:protobuf',
          '//thirdparty/leveldb:leveldb',
         ],
  allow_undefined = True,
)


cc_test(
    name = 'reader_test',
    srcs = [
               'reader_test.cc'
           ],
    deps = [
               ':reader',
               '//app/qzap/common/utility:utility',
               '//common/config:config',
               '//thirdparty/gtest:gtest',
           ],
    testdata =
           [
                ('testdata/line_reader.instance', 'line_reader.instance'),
                ('testdata/line_reader.conf', 'line_reader.conf'),
                ('testdata/xml_reader.instance', 'xml_reader.instance'),
                ('testdata/xml_reader.conf', 'xml_reader.conf'),
           ],
)