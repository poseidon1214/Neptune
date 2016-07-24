// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef RETRIEVAL_EXTRACTOR_WRITE_DATA_FUNCTOR_H_
#define RETRIEVAL_EXTRACTOR_WRITE_DATA_FUNCTOR_H_

#include <vector>
#include <string>

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "common/writer/writer.h"
#include "data_collector/feeder/framework/base_functor.h"
#include "retrieval/extractor/data_message.h"

namespace gdt {

class WriteDataFunctor: public ExtractorBaseFunctor {
 public:
  // 从文件里解析商品数据
  FunctorResult DoWork(ExtratorDataMessage* data_message) {
    if (!Writer::WriteToIO(config_->document_writer_config(),
                           data_message->documents)) {
      return kFailedNotContinued;
    }
    return kSuccess;
  }
};

}  // namespace gdt

#endif  // RETRIEVAL_EXTRACTOR_WRITE_DATA_FUNCTOR_H_
