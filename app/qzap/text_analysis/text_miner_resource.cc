// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/text_miner_resource.h"

#include <tr1/memory>

#include "thirdparty/protobuf/text_format.h"
#include "app/qzap/common/thread/mutex.h"
#include "app/qzap/common/utility/file_utility.h"
#include "app/qzap/text_analysis/classifier/classifier.h"
#include "app/qzap/text_analysis/dict_manager.h"
#include "app/qzap/text_analysis/embedding/embedding_inference_engine.h"
#include "app/qzap/text_analysis/keyword/keyword_extractor.h"
#include "app/qzap/text_analysis/segmenter/segmenter.h"
#include "app/qzap/text_analysis/text_miner.pb.h"
#include "app/qzap/text_analysis/token/token_extractor.h"
#include "app/qzap/text_analysis/topic/topic_inference_engine.h"

DEFINE_string(text_miner_resource_config_file,
              "testdata/text_miner_resource.config",
              "the text_miner_resource config filename,"
              "maybe contain multi-resource_config");
DEFINE_bool(text_miner_resource_lazy_init, true,
            "Enable lazy initialization of text miner resources.");

namespace qzap {
namespace text_analysis {

TextMinerResource::TextMinerResource()
  : is_init_dict_manager_(false),
    is_init_segmenter_(false),
    is_init_token_extractor_(false),
    is_init_keyword_extractor_(false),
    is_init_topic_inference_engine_(false),
    is_init_embedding_inference_engine_(false) {
}

TextMinerResource::TextMinerResource(const std::string& config_file)
  : is_init_dict_manager_(false),
    is_init_segmenter_(false),
    is_init_token_extractor_(false),
    is_init_keyword_extractor_(false),
    is_init_topic_inference_engine_(false),
    is_init_embedding_inference_engine_(false) {
  CHECK(InitFromConfigFile(config_file));
}

bool TextMinerResource::InitFromConfigFile(const std::string& config_file) {
  std::string text;
  if (!ReadFileToString(config_file, &text)) {
    LOG(ERROR) << "Failed to load from file: " << config_file;
    return false;
  }

  ResourceConfig resource_config;
  if (!google::protobuf::TextFormat::ParseFromString(text, &resource_config)) {
    LOG(ERROR) << "Failed to parse string: " << text;
    return false;
  }
  return InitFromPbMessage(resource_config);
}

bool TextMinerResource::InitFromPbMessage(
    const ResourceConfig& resource_config) {
  resource_config_.CopyFrom(resource_config);

  if (FLAGS_text_miner_resource_lazy_init) {
    // lazy init
    return true;
  } else {
    // NOTE(fandywang): 不要直接调用 InitXXX，因为未加锁，如果多线程调用，
    // 同时请求 GetXXX 会 Core Dump
    return (GetDictManager() != NULL && GetTokenExtractor() != NULL
            && GetKeywordExtractor() != NULL && GetTopicInferenceEngine() != NULL
          && GetClassifier() != NULL && GetEmbeddingInferenceEngine() != NULL);
  }
}

TextMinerResource::~TextMinerResource() { Clear(); }

void TextMinerResource::Clear() {
  dict_manager_.reset();
  segmenter_.reset();
  token_extractor_.reset();
  keyword_extractor_.reset();
  topic_inference_engine_.reset();
  embedding_inference_engine_.reset();
  for (ClassifierMapIter iter = classifier_map_.begin();
       iter != classifier_map_.end(); ++iter) {
    delete iter->second;
  }
  classifier_map_.clear();

  is_init_dict_manager_ = false;
  is_init_segmenter_ = false;
  is_init_token_extractor_ = false;
  is_init_keyword_extractor_ = false;
  is_init_topic_inference_engine_ = false;
  is_init_embedding_inference_engine_ = false;
}

const DictManager* TextMinerResource::GetDictManager() {
  if (!is_init_dict_manager_) {
    MutexLock locker(&dict_manager_mutex_);
    if (!is_init_dict_manager_ && !InitDictManager()) {
      return NULL;
    }
  }
  return dict_manager_.get();
}

const Segmenter* TextMinerResource::GetSegmenter() {
  if (!is_init_segmenter_) {
    MutexLock locker(&module_mutex_);
    if (!is_init_segmenter_ && !InitSegmenter()) {
      return NULL;
    }
  }
  return segmenter_.get();
}

const TokenExtractor* TextMinerResource::GetTokenExtractor() {
  if (!is_init_token_extractor_) {
    MutexLock locker(&module_mutex_);
    if (!is_init_token_extractor_ && !InitTokenExtractor()) {
      return NULL;
    }
  }
  return token_extractor_.get();
}

const KeywordExtractor* TextMinerResource::GetKeywordExtractor() {
  if (!is_init_keyword_extractor_) {
    MutexLock locker(&module_mutex_);
    if (!is_init_keyword_extractor_ && !InitKeywordExtractor()) {
      return NULL;
    }
  }
  return keyword_extractor_.get();
}

const TopicInferenceEngine* TextMinerResource::GetTopicInferenceEngine() {
  if (!is_init_topic_inference_engine_) {
    MutexLock locker(&module_mutex_);
    if (!is_init_topic_inference_engine_ && !InitTopicInferenceEngine()) {
      return NULL;
    }
  }
  return topic_inference_engine_.get();
}

const Classifier* TextMinerResource::GetClassifier() {
  // 根据目前resource_config_中的resource_name查找相应的Classifier
  ClassifierMapIter iter = classifier_map_.find(
      resource_config_.resource_name());

  if (iter == classifier_map_.end()) {
    MutexLock locker(&module_mutex_);
    if (!InitClassifier()) {
      return NULL;
    }

    iter = classifier_map_.find(resource_config_.resource_name());
    if (iter == classifier_map_.end()) {
      return NULL;
    }
  }

  return iter->second;  // 返回classifier_map_中resource_name对应的Classifier
}

const EmbeddingInferenceEngine* TextMinerResource::GetEmbeddingInferenceEngine() {
  if (!is_init_embedding_inference_engine_) {
    MutexLock locker(&module_mutex_);
    if (!is_init_embedding_inference_engine_
        && !InitEmbeddingInferenceEngine()) {
      return NULL;
    }
  }
  return embedding_inference_engine_.get();
}

bool TextMinerResource::InitDictManager() {
  dict_manager_.reset(new DictManager());
  if (!resource_config_.has_dict_dir()) {
    LOG(WARNING) << "dict_dir is NULL.";
    return false;
  }
  if (!dict_manager_->Init(resource_config_.dict_dir())) {
    LOG(ERROR) << "DictManager initialization failed.";
    return false;
  }
  is_init_dict_manager_ = true;
  return true;
}

bool TextMinerResource::InitSegmenter() {
  segmenter_.reset(new Segmenter());
  if (!segmenter_->Init()) {
    LOG(ERROR) << "Segmenter initialization failed.";
    return false;
  }
  is_init_segmenter_ = true;
  return true;
}

bool TextMinerResource::InitTokenExtractor() {
  const DictManager* dict_manager = GetDictManager();
  token_extractor_.reset(new TokenExtractor(*dict_manager));
  is_init_token_extractor_ = true;
  return true;
}

bool TextMinerResource::InitKeywordExtractor() {
  const DictManager* dict_manager = GetDictManager();
  keyword_extractor_.reset(new KeywordExtractor(*dict_manager));
  is_init_keyword_extractor_ = true;
  return true;
}

bool TextMinerResource::InitTopicInferenceEngine() {
  topic_inference_engine_.reset(new TopicInferenceEngine());
  if (!resource_config_.has_peacock_model_dir()) {
    LOG(WARNING) << "peacock_model_dir is NULL.";
    return false;
  }
  if (!topic_inference_engine_->LoadModel(
          resource_config_.peacock_model_dir())) {
    LOG(ERROR) << "LDAInferenceEngine initialization failed.";
    return false;
  }
  is_init_topic_inference_engine_ = true;
  return true;
}

bool TextMinerResource::InitClassifier() {
  Classifier* classifier = new Classifier();
  if (!resource_config_.has_classifier_model_dir()) {
    LOG(WARNING) << "classifier_model_dir is NULL.";
    return false;
  }
  if (!classifier->LoadModel(resource_config_.classifier_model_dir())) {
    LOG(ERROR) << "Classifier initialization failed.";
    return false;
  }
  classifier_map_[resource_config_.resource_name()] = classifier;
  return true;
}

bool TextMinerResource::InitEmbeddingInferenceEngine() {
  const DictManager* dict_manager = GetDictManager();
  embedding_inference_engine_.reset(
      new EmbeddingInferenceEngine(*dict_manager));
  is_init_embedding_inference_engine_ = true;
  return true;
}

}  // namespace text_analysis
}  // namespace qzap

