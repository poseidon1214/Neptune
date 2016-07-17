// Copyright 2015 Tencent Inc.
// Author: Guangneng Hu (lesliehu@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)
//
// Embedding Inference 类：将 doc 用 embedding 低维语义空间向量表示

#ifndef APP_QZAP_TEXT_ANALYSIS_EMBEDDING_EMBEDDING_INFERENCE_ENGINE_H_
#define APP_QZAP_TEXT_ANALYSIS_EMBEDDING_EMBEDDING_INFERENCE_ENGINE_H_

#include "common/base/uncopyable.h"

namespace qzap {
namespace text_analysis {

class DictManager;  // 词典管理器
class Document;  // 文档类

class EmbeddingInferenceEngine {
 public:
  explicit EmbeddingInferenceEngine(const DictManager& dict_manager);
  ~EmbeddingInferenceEngine();

  // 在分词和命名实体识别结果基础上, 抽取 BoW, 利用训练好的 Word Embedding
  // 模型, Inference doc embedding 表示
  bool Infer(Document* document) const;

 private:
  // 词典管理器句柄, 主要使用 word_embedding_dict
  const DictManager& dict_manager_;

  DECLARE_UNCOPYABLE(EmbeddingInferenceEngine);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_EMBEDDING_EMBEDDING_INFERENCE_ENGINE_H_

