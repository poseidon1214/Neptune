// Copyright (c) 2014, Tencent Inc.
// Author: <cernwang@tencent.com>

// Copyright (c) 2015 Tencent Inc.
// Author: Wang Qian (cernwang@tencent.com)

#include "common/config/base_config_manager.h"
#include "retrieval/extractor/extractor_register.h"
#include "framework/state_machine/task.h"
#include "framework/state_machine/state_machine.h"

using namespace gdt;
using namespace gdt::common;

DEFINE_string(framework_conf, "../conf/extract_framework.conf", "状态机配置路径");
DEFINE_string(extractor_conf, "../conf/extractor.conf", "配制文件路径");

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging(argv[0]);
  StateMachine<ExtractorConfig, ExtratorDataMessage> state_machine;
  CHECK(BaseConfigManager<StateMachineConfig>::Instance().Load(FLAGS_framework_conf), FLAGS_framework_conf);
  CHECK(BaseConfigManager<ExtractorConfig>::Instance().Load(FLAGS_extractor_conf), FLAGS_extractor_conf);
  ExtratorDataMessage data_message;
  state_machine.Sync(&data_message, &BaseConfigManager<ExtractorConfig>::Instance().Get());
  CHECK(state_machine.Init(BaseConfigManager<StateMachineConfig>::Instance().Get()), FLAGS_framework_conf);
  return state_machine.Run();
}
