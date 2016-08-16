// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>


#include "learning/extractor/extractor.h"
#include <algorithm>

namespace gdt {
namespace learning {


bool Extractor::Init(const ExtractorConfig& extractor_config) {
  extractor_config_.CopyFrom(extractor_config);
  return true;
}

bool Extractor::Extract(const google::protobuf::Message& message,
                        std::map<std::string, FieldValue>* features) {
  std::for_each(extractor_config_.feature_config().begin(),
  	            extractor_config_.feature_config().end(),
  	            std::bind(&Extractor::ExtractFeature, this, &message,
  	            	      std::placeholders::_1, features));
  return true;
}

bool Extractor::ExtractFeature(const google::protobuf::Message* message,
                               const FeatureConfig& feature_config,
                               std::map<std::string, FieldValue>* feature) {
  return true;
}

}  // namespace learning
}  // namespace gdt