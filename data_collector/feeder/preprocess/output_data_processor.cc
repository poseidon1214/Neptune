// Copyright (c) 2015, Tencent Inc.
// Author: cernwang <cernwang@tencent.com>

#include "data_collector/feeder/preprocess/output_data_processor.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"
#include "common/base/shared_ptr.h"
#include "data_collector/feeder/preprocess/write_data_functor.h"

namespace gdt {

bool OutputDataProcessor::Init() {
  INIT_FUNCTOR(write_data_functor, WriteDataFunctor);
  success_init_ = true;
  return true;
}

}  // namespace gdt
