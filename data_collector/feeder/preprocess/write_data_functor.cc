// Copyright (c) 2015, Tencent Inc.
// Author: cernwang <cernwang@tencent.com>

#include "data_collector/feeder/preprocess/write_data_functor.h"

#include <time.h>
#include "common/writer/writer.h"
#include "data_collector/feeder/common/rules_common.h"


namespace gdt {

bool WriteDataFunctor::Init() {
  return true;
}

FunctorResult WriteDataFunctor::DoWork(DataMessage* data_message) {
  if (!Writer::WriteToIO(config_->product_writer_config(), data_message->products)) {
    return kFailedNotContinued;
  }
  return kSuccess;
}


}  // namespace gdt
