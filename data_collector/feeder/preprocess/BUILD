cc_library(
  name = 'parse_data_processor',
  srcs = [
           'parse_data_processor.cc',
         ],
  deps = [
           '//app/qzap/common/utility:utility',
           '//common/base/string:string',
           ':read_data_functor',
         ],
)

cc_library(
  name = 'read_data_functor',
  srcs = [
           'read_data_functor.cc',
         ],
  deps = [
           '//app/qzap/common/utility:utility',
           '//common/base/string:string',
           '//data_collector/proto:feeder_config_pb',
           '//data_collector/proto:product_pb',        
         ],
  allow_undefined = True,
)

cc_library(
  name = 'output_data_processor',
  srcs = [
           'output_data_processor.cc',
         ],
  deps = [
           '//app/qzap/common/utility:utility',
           '//common/base/string:string',
           ':write_data_functor',
         ],
)


cc_library(
  name = 'write_data_functor',
  srcs = [
           'write_data_functor.cc',
         ],
  deps = [
           '//app/qzap/common/utility:utility',
           '//common/base/string:string',
           '//common/writer:writer',
           '//data_collector/proto:feeder_config_pb',
           '//data_collector/proto:product_pb',
         ],
  allow_undefined = True,
)

cc_binary(
  name = 'feeder_main',
  srcs = [
           'feeder_main.cc',
         ],
  deps = [
           ':output_data_processor',
           ':parse_data_processor',
           '//common/config:config',
           '//data_collector/feeder/get_data:get_data_processor',
         ],
)