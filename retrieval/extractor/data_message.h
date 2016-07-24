// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef RETRIEVAL_EXTRACTOR_DATA_MESSAGE_H_
#define RETRIEVAL_EXTRACTOR_DATA_MESSAGE_H_

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include "retrieval/proto/extract.pb.h"
#include "retrieval/proto/document.pb.h"
#include "framework/common/base_processor.h"

namespace gdt {

struct ExtratorDataMessage {
  // 数据
  std::vector<Product> products;
  // 文档
  std::vector<Document> documents;
};

typedef  BaseFunctor<ExtractorConfig, ExtratorDataMessage> ExtractorBaseFunctor;

}  // namespace gdt

#endif  // RETRIEVAL_EXTRACTOR_DATA_MESSAGE_H_

