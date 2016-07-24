cc_test(
  name = 'state_machine_test',
  srcs = [
           'state_machine_test.cc',
         ],
  deps = [
           '//common/config:config',
           '//data_collector/proto:feeder_config_pb',
           '//framework/proto:framework_pb',
           '//thirdparty/glog:glog',
           '//thirdparty/gflags:gflags',
           '//thirdparty/gtest:gtest',
         ],
  testdata =
         [
            ('testdata/state_machine.conf', 'state_machine.conf'),
            ('testdata/feeder.conf', 'feeder.conf'),
         ],
)