// Copyright (c) 2015, Tencent Inc.
// Author: cernwang<cernwang@tencent.com>

#ifndef DATA_COLLECTOR_FEEDER_PREPROCESS_PARSE_DATA_PROCESSOR_H_
#define DATA_COLLECTOR_FEEDER_PREPROCESS_PARSE_DATA_PROCESSOR_H_

#include <string>

#include "data_collector/feeder/framework/base_processor.h"
#include "data_collector/feeder/framework/data_message.h"

namespace gdt {

class ParseDataProcessor : public FeederBaseProcessor {
 public:
  ParseDataProcessor() {}

  virtual ~ParseDataProcessor() {
  }

  virtual bool Init();

  std::string ProcessorName() {
    return "ParseDataProcessor";
  }
};

}  // namespace gdt
#endif  // DATA_COLLECTOR_FEEDER_PREPROCESS_PARSE_DATA_PROCESSOR_H_

