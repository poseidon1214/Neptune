// Copyright (c) 2015 Tencent Inc.
// Author: Wang Qian (cernwang@tencent.com)

#ifndef COMMON_STATE_MACHINE_REGISTER_H_
#define COMMON_STATE_MACHINE_REGISTER_H_

#include <map>
#include <utility>
#include <vector>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

#include "app/qzap/common/base/base.h"
#include "common/base/class_register.h"
#include "data_collector/feeder/framework/data_message.h"
#include "framework/common/base_processor.h"

namespace gdt {
namespace common {

typedef  BaseFunctor<FeederConfig, DataMessage> Functor;
class TestFunctor : public Functor {

  FunctorResult DoWork(DataMessage* data_message) {
    LOG(ERROR) << "Test Functor DoWork";
    return kSuccess;
  }

};

CLASS_REGISTER_DEFINE_REGISTRY(Functor_register, Functor);
REGISTER_FUNCTOR(TestFunctor, Functor);

}  // namespace common
}  // namespace gdt

#endif  // COMMON_STATE_MACHINE_REGISTER_H_
