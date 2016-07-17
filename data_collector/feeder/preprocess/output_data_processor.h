// Copyright (c) 2015, Tencent Inc.
// Author: cernwang<cernwang@tencent.com>

#ifndef DATA_COLLECTOR_FEEDER_PREPROCESS_OUTPUT_DATA_PROCESSOR_H_
#define DATA_COLLECTOR_FEEDER_PREPROCESS_OUTPUT_DATA_PROCESSOR_H_

#include <string>

#include "data_collector/feeder/framework/base_processor.h"
#include "data_collector/feeder/framework/data_message.h"

namespace gdt {

class OutputDataProcessor : public FeederBaseProcessor {
 public:
  OutputDataProcessor() {}

  virtual ~OutputDataProcessor() {
  }

  virtual bool Init();

  std::string ProcessorName() {
    return "OutputDataProcessor";
  }
};

}  // namespace gdt
#endif  // DATA_COLLECTOR_FEEDER_PREPROCESS_OUTPUT_DATA_PROCESSOR_H_

