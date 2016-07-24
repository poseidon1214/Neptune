// Copyright (c) 2014, Tencent Inc.
// Author: <cernwang@tencent.com>

// Copyright (c) 2015 Tencent Inc.
// Author: Wang Qian (cernwang@tencent.com)

#include "common/config/base_config_manager.h"
#include "framework/state_machine/register_test.h"
#include "framework/state_machine/task.h"
#include "framework/state_machine/state_machine.h"
#include "retrieval/proto/extract.pb.h"
#include "retrieval/proto/document.pb.h"

using namespace gdt::common;

DEFINE_string(framework_conf, "../conf/extract_framework.conf", "状态机配置路径");
DEFINE_string(extractor_conf, "../conf/extractor.conf", "配制文件路径");

struct ExtratorDataMessage {
  // 数据
  std::vector<Product> data;
  // 文档
  std::vector<Document> document;
};

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging(argv[0]);
  StateMachine<ExtractorConfig, ExtratorDataMessage> state_machine;
  CHECK(BaseConfigManager<StateMachineConfig>::Instance().Load(FLAGS_framework_conf));
  CHECK(BaseConfigManager<ExtractorConfig>::Instance().Load(FLAGS_extractor_conf));
  ExtratorDataMessage data_message;
  state_machine.Sync(&data_message, &BaseConfigManager<FeederConfig>::Instance().Get());
  CHECK(state_machine.Init(BaseConfigManager<StateMachineConfig>::Instance().Get()));
  return state_machine.Run();
}
