// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef DATA_COLLECTOR_FEEDER_PREPROCESS_WRITE_DATA_FUNCTOR_H_
#define DATA_COLLECTOR_FEEDER_PREPROCESS_WRITE_DATA_FUNCTOR_H_

#include <vector>
#include <string>

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "data_collector/feeder/framework/base_functor.h"
#include "data_collector/feeder/framework/data_message.h"

namespace gdt {

class WriteDataFunctor: public FeederBaseFunctor {
 public:
  WriteDataFunctor() : FeederBaseFunctor() {
  }

  virtual bool Init();

  virtual ~WriteDataFunctor() {}

  // 从文件里解析商品数据
  FunctorResult DoWork(DataMessage* data_message);

};

}  // namespace gdt

#endif  // DATA_COLLECTOR_FEEDER_PREPROCESS_WRITE_DATA_FUNCTOR_H_
