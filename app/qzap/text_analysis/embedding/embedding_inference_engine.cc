// Copyright 2015 Tencent Inc.
// Author: Guangneng Hu (lesliehu@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/embedding/embedding_inference_engine.h"

#include <stdint.h>
#include <string>

#include "thirdparty/gflags/gflags.h"
#include "app/qzap/text_analysis/dict/word_embedding_dict.h"
#include "app/qzap/text_analysis/dict_manager.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

namespace qzap {
namespace text_analysis {

static const double kEpsilon = 1E-6;

EmbeddingInferenceEngine::EmbeddingInferenceEngine(
    const DictManager& dict_manager) : dict_manager_(dict_manager) {}

EmbeddingInferenceEngine::~EmbeddingInferenceEngine() {}

bool EmbeddingInferenceEngine::Infer(Document* document) const {
  if (!document->has_extracted_token()) {
    LOG(WARNING) << "The document must be extract bow_token first.";
    return false;
  }

  const WordEmbeddingDict* word_embedding_dict
      = dict_manager_.GetWordEmbeddingDict();
  if (word_embedding_dict == NULL) {
    LOG(ERROR) << "word_embedding_dict is NULL.";
    return false;
  }

  // Doc Embedding: sum(word_embedding) / token_size
  // 实验验证以上方法应用于分类器效果最佳
  int token_size = 0;
  for (int i = 0; i < document->bow_token_size(); ++i) {
    const Token& bow_token = document->bow_token(i);
    const EmbeddingInfo* embedding_info
        = word_embedding_dict->Search(bow_token.text());
    if (embedding_info == NULL) { continue; }

    token_size += 1;
    if (document->embedding_size() == 0) {
      for (int j = 0; j < embedding_info->embedding_size(); ++j) {
        Embedding* embedding = document->add_embedding();
        // sum (word_embedding)
        embedding->set_ori_weight(embedding_info->embedding(j));
      }
    } else {
      for (int j = 0; j < embedding_info->embedding_size(); ++j) {
        Embedding* embedding = document->mutable_embedding(j);
        embedding->set_ori_weight(
            embedding->ori_weight() + embedding_info->embedding(j));
      }
    }
  }
  document->set_has_infered_embedding(true);

  if (document->bow_token_size() > 0) {
    // TODO(neoma,fandywang): weight 是否需要做些转化？
    for (int i = 0; i < document->embedding_size(); ++i) {
      Embedding* embedding = document->mutable_embedding(i);
      embedding->set_weight(embedding->ori_weight() / token_size);
    }
  }

  return true;
}

}  // namespace text_analysis
}  // namespace qzap

