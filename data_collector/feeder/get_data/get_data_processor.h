// Copyright (c) 2015, Tencent Inc.
// Author: cernwang<cernwang@tencent.com>

#ifndef DATA_COLLECTOR_FEEDER_GET_DATA_GET_DATA_PROCESSOR_H_
#define DATA_COLLECTOR_FEEDER_GET_DATA_GET_DATA_PROCESSOR_H_

#include <string>

#include "data_collector/feeder/framework/base_processor.h"
#include "data_collector/feeder/framework/data_message.h"

namespace gdt {

class GetDataProcessor : public FeederBaseProcessor {
 public:
  GetDataProcessor() {}

  virtual ~GetDataProcessor() {
  }

  virtual bool Init();

  std::string ProcessorName() {
    return "GetDataProcessor";
  }
};

}  // namespace gdt
#endif  // DATA_COLLECTOR_FEEDER_GET_DATA_GET_DATA_PROCESSOR_H_

