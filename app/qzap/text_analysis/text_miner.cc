// Copyright (c) 2012 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/text_miner.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "app/qzap/text_analysis/classifier/classifier.h"
#include "app/qzap/text_analysis/dict_manager.h"
#include "app/qzap/text_analysis/embedding/embedding_inference_engine.h"
#include "app/qzap/text_analysis/keyword/keyword_extractor.h"
#include "app/qzap/text_analysis/segmenter/segmenter.h"
#include "app/qzap/text_analysis/text_miner.pb.h"
#include "app/qzap/text_analysis/text_miner_resource.h"
#include "app/qzap/text_analysis/token/token_extractor.h"
#include "app/qzap/text_analysis/topic/topic_inference_engine.h"

namespace qzap {
namespace text_analysis {

TextMiner::TextMiner(TextMinerResource* text_miner_resource)
    : text_miner_resource_(text_miner_resource) {
}

TextMiner::~TextMiner() {}

bool TextMiner::Segment(Document* document) const {
  if (document->has_segmented()) {
    return true;
  }

  const Segmenter* segmenter = text_miner_resource_->GetSegmenter();
  const StopwordDict* stopword_dict
      = text_miner_resource_->GetDictManager()->GetStopwordDict();
  if (segmenter == NULL || stopword_dict == NULL) {
    return false;
  }

  std::vector<std::string> words;
  std::vector<std::string> word_types;
  bool has_new_field = false;
  for (int i = 0; i < document->field_size(); ++i) {
    Field* field = document->mutable_field(i);
    static float kEpsilon = 1E-6;
    if (field->weight() < kEpsilon) {  // 权重为 0 的字段不予处理
      continue;
    }
    if (field->token_size() > 0) {  // 已经完成分词
      continue;
    }
    has_new_field = true;
    words.clear();
    word_types.clear();
    if (!segmenter->SegmentWithWordType(field->text(), &words, &word_types)) {
      return false;
    }

    uint32_t current_offset = 0;
    for (uint32_t j = 0; j < words.size(); ++j) {
      TokenOccurence* token = field->add_token();
      token->set_text(words[j]);
      token->set_word_type(word_types[j]);
      token->set_offset(current_offset);
      token->set_is_stopword(stopword_dict->IsStopword(words[j]));
      current_offset += words[j].size();
    }
  }
  document->set_has_segmented(true);
  if (has_new_field) {
    document->clear_bow_token();
    document->set_has_extracted_token(false);
  }

  document->set_resource_name(text_miner_resource_->GetResourceName());

  return true;
}

bool TextMiner::ExtractTokens(Document* document) const {
  if (document->has_extracted_token()) {
    return true;
  }

  if (!document->has_segmented() && !Segment(document)) {
    return false;
  }

  const TokenExtractor* token_extractor
      = text_miner_resource_->GetTokenExtractor();
  if (token_extractor == NULL || !token_extractor->Extract(document)) {
    return false;
  }
  return true;
}

bool TextMiner::ExtractKeywords(Document* document) const {
  if (document->has_extracted_keyword()) {
    return true;
  }

  if (!document->has_segmented() && !Segment(document)) {
    return false;
  }

  const KeywordExtractor* keyword_extractor
      = text_miner_resource_->GetKeywordExtractor();
  if (keyword_extractor == NULL || !keyword_extractor->Extract(document)) {
    return false;
  }
  return true;
}

bool TextMiner::InferTopics(Document* document) const {
  if (document->has_infered_topic()) {
    return true;
  }

  if (!document->has_extracted_token() && !ExtractTokens(document)) {
    return false;
  }

  const TopicInferenceEngine* topic_inference_engine
      = text_miner_resource_->GetTopicInferenceEngine();
  if (!topic_inference_engine->InferAndExplain(document)) {
    return false;
  }

  return true;
}

bool TextMiner::ExplainTopicWords(Document* document) const {
  if (document->has_explained_topic_word()) {
    return true;
  }
  return InferTopics(document);
}

bool TextMiner::Classify(Document* document) const {
  if (document->has_classified()) {
    return true;
  }

  if (!document->has_extracted_token() && !ExtractTokens(document)) {
    return false;
  }
  if (!document->has_infered_topic() && !InferTopics(document)) {
    return false;
  }

  const Classifier* classifier = text_miner_resource_->GetClassifier();
  if (!classifier->Predict(document)) {
    return false;
  }
  document->set_has_classified(true);

  return true;
}

bool TextMiner::InferEmbedding(Document* document) const {
  if (document->has_infered_embedding()) {
    return true;
  }

  if (!document->has_extracted_token() && !ExtractTokens(document)) {
    return false;
  }

  const EmbeddingInferenceEngine* embedding_inference_engine
      = text_miner_resource_->GetEmbeddingInferenceEngine();
  if (!embedding_inference_engine->Infer(document)) {
    return false;
  }
  document->set_has_infered_embedding(true);

  return true;
}

}  // namespace text_analysis
}  // namespace qzap
