// Copyright 2015 Tencent Inc.
// Author: Guangneng Hu (lesliehu@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)
//
// Word Embedding 词向量词典类

#ifndef APP_QZAP_TEXT_ANALYSIS_DICT_WORD_EMBEDDING_DICT_H_
#define APP_QZAP_TEXT_ANALYSIS_DICT_WORD_EMBEDDING_DICT_H_

#include <string>

#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/dict/dict.pb.h"
#include "app/qzap/text_analysis/dict/strkey_value_dict_base.h"

namespace qzap {
namespace text_analysis {

class WordEmbeddingDict : public StrKeyValueDictBase<EmbeddingInfo> {
 public:
  WordEmbeddingDict() {}
  ~WordEmbeddingDict() {}

  bool Build(const std::string& filename);

 private:
  DECLARE_UNCOPYABLE(WordEmbeddingDict);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_DICT_WORD_EMBEDDING_DICT_H_
