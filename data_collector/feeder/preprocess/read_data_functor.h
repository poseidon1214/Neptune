// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef DATA_COLLECTOR_FEEDER_PREPROCESS_READ_DATA_FUNCTOR_H_
#define DATA_COLLECTOR_FEEDER_PREPROCESS_READ_DATA_FUNCTOR_H_

#include <vector>
#include <string>

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "data_collector/feeder/framework/base_functor.h"
#include "data_collector/feeder/framework/data_message.h"

namespace gdt {

class ReadDataFunctor: public FeederBaseFunctor {
 public:
  ReadDataFunctor() : FeederBaseFunctor() {
  }

  virtual bool Init();

  virtual ~ReadDataFunctor() {}

  // 从文件里解析商品数据
  FunctorResult DoWork(DataMessage* data_message);

 private:
  bool GetProductFromFeedFile(
      const FeederFile& feeder_file,
      std::vector<Product>* products);

 public:
  std::map<size_t, size_t> source_to_index_;
};

}  // namespace gdt

#endif  // DATA_COLLECTOR_FEEDER_PREPROCESS_READ_DATA_FUNCTOR_H_
