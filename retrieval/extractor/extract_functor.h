// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef RETRIEVAL_EXTRACTOR_EXTRACTOR_FUNCTOR_H_
#define RETRIEVAL_EXTRACTOR_EXTRACTOR_FUNCTOR_H_

#include <vector>
#include <string>

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "common/reader/reader.h"
#include "retrieval/extractor/data_message.h"

namespace gdt {

class ExtractFunctor: public ExtractorBaseFunctor {
 public:
  // 从文件里解析商品数据
  FunctorResult DoWork(ExtratorDataMessage* data_message) {
    std::for_each(data_message->products.begin(),
    	          data_message->products.end(),
    	          [=](const Product& product) {
    	          	Document document;
    	          	Convert(product, &document);
                    data_message->documents.push_back(document);
    	          });
    return kSuccess;
  }
 private:
  // 讲消息转换成文档
  bool Convert(const Product& product, Document* document) {
    return true;
  }
};

}  // namespace gdt

#endif  // RETRIEVAL_EXTRACTOR_EXTRACTOR_FUNCTOR_H_
