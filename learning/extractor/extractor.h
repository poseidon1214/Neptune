// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

// 对商品数据库的操作的类
#ifndef LEARNING_CLEANER_LAMBDA_PARSER_H_
#define LEARNING_CLEANER_LAMBDA_PARSER_H_


#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include "common/proto/config.pb.h"
#include "common/encoding/proto_converter.h"
#include "framework/common/base_functor.h"
#include "learning/cleaner/condition.h"
#include "learning/proto/condition.pb.h"
#include "learning/cleaner/lambda_parser.h"
#include "learning/proto/test.pb.h"
#include "learning/proto/extractor.pb.h"

namespace gdt {
namespace learning {

class Extractor {
 public:
  bool Init(const ExtractorConfig& extractor_config);
  bool Extract(const google::protobuf::Message& message,
               std::map<std::string, FieldValue>* features);
  bool ExtractFeature(const google::protobuf::Message* message,
                      const FeatureConfig& feature_config,
                      std::map<std::string, FieldValue>* feature);
 private:
  ExtractorConfig extractor_config_;
};

}  // namespace learning
}  // namespace gdt

#endif  // LEARNING_CLEANER_LAMBDA_PARSER_H_
