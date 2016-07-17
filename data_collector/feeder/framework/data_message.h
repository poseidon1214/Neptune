// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef FRAMEWORK_DATA_MESSAGE_H_
#define FRAMEWORK_DATA_MESSAGE_H_

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include "data_collector/proto/product.pb.h"
#include "data_collector/proto/feeder_config.pb.h"

namespace gdt {

// 从前往后传递的数据流消息
class DataMessage {
 public:
  // 商品
  std::vector<Product> products;
  // 商品
  std::vector<FeederFile> feeder_files;
};

}  // namespace gdt

#endif  // FRAMEWORK_DATA_MESSAGE_H_

