// Copyright (c) 2015 Tencent Inc.
// Author: Wang Qian (cernwang@tencent.com)

#include "thirdparty/gtest/gtest.h"
#include "framework/state_machine/register_test.h"
#include "framework/state_machine/task.h"
#include "framework/state_machine/state_machine.h"
#include "common/config/base_config_manager.h"

namespace gdt {
namespace common {

TEST(StateMachine, StateMachineTest) {
  StateMachine<FeederConfig, DataMessage> sm;
  EXPECT_TRUE(BaseConfigManager<StateMachineConfig>::Instance().Load("state_machine.conf"));
  EXPECT_TRUE(BaseConfigManager<FeederConfig>::Instance().Load("feeder.conf"));
  DataMessage dm;
  sm.Sync(&dm, &BaseConfigManager<FeederConfig>::Instance().Get());
  EXPECT_TRUE(sm.Init(BaseConfigManager<StateMachineConfig>::Instance().Get()));
  sm.Run();
}

}  // namespace common
}  // namespace gdt