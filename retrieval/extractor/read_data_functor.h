// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef RETRIEVAL_EXTRACTOR_READ_DATA_FUNCTOR_H_
#define RETRIEVAL_EXTRACTOR_READ_DATA_FUNCTOR_H_

#include <vector>
#include <string>

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "common/reader/reader.h"
#include "retrieval/extractor/data_message.h"

namespace gdt {

class ReadDataFunctor: public ExtractorBaseFunctor {
 public:
  // 从文件里解析商品数据
  FunctorResult DoWork(ExtratorDataMessage* data_message) {
    if (!Reader::ReadFromIO(config_->product_reader_config(),
                            &(data_message->products))) {
      return kFailedNotContinued;
    }
    return kSuccess;
  }
};

}  // namespace gdt

#endif  // RETRIEVAL_EXTRACTOR_READ_DATA_FUNCTOR_H_
