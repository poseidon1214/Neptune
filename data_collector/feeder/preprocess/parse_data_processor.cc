// Copyright (c) 2015, Tencent Inc.
// Author: cernwang <cernwang@tencent.com>

#include "data_collector/feeder/preprocess/parse_data_processor.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"
#include "common/base/shared_ptr.h"
#include "data_collector/feeder/preprocess/read_data_functor.h"

namespace gdt {

bool ParseDataProcessor::Init() {
  INIT_FUNCTOR(read_data_functor, ReadDataFunctor);
  success_init_ = true;
  return true;
}

}  // namespace gdt
