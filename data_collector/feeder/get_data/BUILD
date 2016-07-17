cc_library(
  name = 'get_data_processor',
  srcs = [
           'get_data_processor.cc',
         ],
  deps = [
           '//app/qzap/common/utility:utility',
           '//common/base/string:string',
           ':index_info_load_functor',
           ':raw_data_download_functor',
         ],
)


cc_library(
  name = 'index_info_load_functor',
  srcs = [
           'index_info_load_functor.cc',
         ],
  deps = [
           '//app/qzap/common/utility:utility',
           '//common/base/string:string',
           '//common/net/http:downloader',
           '//common/file:file_tools',
           '//common/reader:reader',
           '//data_collector/proto:feeder_config_pb',
           '//data_collector/feeder/common:rules_common',
         ],
  allow_undefined = True,
)

cc_library(
  name = 'raw_data_download_functor',
  srcs = [
           'raw_data_download_functor.cc',
         ],
  deps = [
           '//app/qzap/common/utility:utility',
           '//common/base/string:string',
           '//common/net/http:downloader',
           '//data_collector/proto:feeder_config_pb',
           '//data_collector/common:curl_downloader',
         ],
  allow_undefined = True,
)