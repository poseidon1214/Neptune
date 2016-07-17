// Copyright (c) 2011 Tencent Inc.
// Author: Pan Yang (baileyyang@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/classifier/feature/feature_extractor.h"

#include <tr1/unordered_map>

#include "thirdparty/glog/logging.h"
#include "thirdparty/protobuf/repeated_field.h"

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/classifier/instance.h"
#include "app/qzap/text_analysis/dict/vocabulary.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

namespace qzap {
namespace text_analysis {

using google::protobuf::RepeatedPtrField;

FeatureExtractor::FeatureExtractor() {}

FeatureExtractor::~FeatureExtractor() {}

// 加载feature词典
bool FeatureExtractor::LoadFeatureVocab(const std::string& feature_vocab) {
  vocabulary_.reset(new Vocabulary);
  return vocabulary_->Load(feature_vocab);
}

void FeatureExtractor::ExtractTokenFeature(const Document& document,
                                           Instance* instance) const {
  for (int i = 0; i < document.bow_token_size(); ++i) {
    std::string token_feature;
    StringAppendF(&token_feature, "1-%s", document.bow_token(i).text().c_str());
    AddFeature(token_feature, document.bow_token(i).weight(), instance);
  }
}

void FeatureExtractor::ExtractKeywordFeature(const Document& document,
                                             Instance* instance) const {
  for (int i = 0; i < document.bow_keyword_size(); ++i) {
    std::string keyword_feature;
    StringAppendF(&keyword_feature,
                  "2-%s",
                  document.bow_keyword(i).text().c_str());
    AddFeature(keyword_feature, document.bow_keyword(i).weight(), instance);
  }
}

void FeatureExtractor::ExtractTopicFeature(const Document& document,
                                           Instance* instance) const {
  for (int i = 0; i < document.topic_size(); ++i) {
    std::string topic_feature;
    StringAppendF(&topic_feature, "3-%d", document.topic(i).id());
    AddFeature(topic_feature, document.topic(i).weight(), instance);
  }
}

void FeatureExtractor::ExtractEmbeddingFeature(const Document& document,
                                               Instance* instance) const {
  for (int i = 0; i < document.embedding_size(); ++i) {
    std::string embedding_feature;
    StringAppendF(&embedding_feature, "4-%d", i);
    AddFeature(embedding_feature, document.embedding(i).weight(), instance);
  }
}

void FeatureExtractor::AddFeature(const std::string& feature,
                                  double weight,
                                  Instance* instance) const {
  int id = vocabulary_->WordIndex(feature);
  if (id != -1) { instance->AddFeature(id, weight); }
}

}  // namespace text_analysis
}  // namespace qzap

