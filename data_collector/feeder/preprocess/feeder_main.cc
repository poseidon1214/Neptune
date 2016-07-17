// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include <iostream>
#include <fstream>
#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"
#include "common/base/shared_ptr.h"
#include "common/config/base_config_manager.h"
#include "app/qzap/common/utility/file_utility.h"
#include "app/qzap/common/utility/time_utility.h"
#include "data_collector/feeder/framework/base_manager.h"
#include "data_collector/feeder/get_data/get_data_processor.h"
#include "data_collector/feeder/preprocess/parse_data_processor.h"
#include "data_collector/feeder/preprocess/output_data_processor.h"

DEFINE_string(conf, "../conf/feeder.conf", "配制文件路径");

namespace gdt {

typedef BaseConfigManager<FeederConfig> ConfigChecker;

class FeederManager: public FeederBaseManager {
 public:
  bool Init() {
    PASS_OR_RETURN(LoadConf(FLAGS_conf));
    INIT_PROCESSOR(get_data_processor, GetDataProcessor);
    INIT_PROCESSOR(parse_data_processor, ParseDataProcessor);
    INIT_PROCESSOR(output_data_processor, OutputDataProcessor);
    return true;
  }
};

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging(argv[0]);
  FeederManager feeder_manager;
  PASS_OR_RETURN(feeder_manager.Init());
  PASS_OR_RETURN(feeder_manager.DoProcess());
  feeder_manager.UnInit();
  return 0;
}

}  // namespace gdt

int main(int argc, char** argv) {
  return gdt::main(argc, argv);
}
