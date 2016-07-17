// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// 词典管理器

#ifndef APP_QZAP_TEXT_ANALYSIS_DICT_MANAGER_H_
#define APP_QZAP_TEXT_ANALYSIS_DICT_MANAGER_H_

#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/dict/keyword_dict.h"
#include "app/qzap/text_analysis/dict/stopword_dict.h"
#include "app/qzap/text_analysis/dict/token_idf_dict.h"
#include "app/qzap/text_analysis/dict/word_embedding_dict.h"
#include "app/qzap/text_analysis/dict/vocabulary.h"

namespace qzap {
namespace text_analysis {

// 组织、管理词典资源，初始化方法：
//   FLAGS_dict_dir 指定词典目录，各词典文件名固定，加载所有词典后调用
//   Init函数初始化DictManager
class DictManager {
 public:
  DictManager();
  ~DictManager();

  // 根据dict_dir目录加载所有词典资源, 各词典文件名固定
  bool Init(const std::string& dict_dir);

  const KeywordDict* GetKeywordDict() const;
  const Vocabulary* GetKeywordBlacklist() const;
  const StopwordDict* GetStopwordDict() const;
  const TokenIdfDict* GetTokenIdfDict() const;
  const WordEmbeddingDict* GetWordEmbeddingDict() const;

 private:
  void Clear();

  // 商业关键词词典，主要来自bidterms、compound_word、vertical_word、baike、
  // qq_IME等词条商业价值较高，用于触发相关性广告
  KeywordDict* keyword_dict_;
  // 关键词黑名单词典
  Vocabulary* keyword_blacklist_;

  // 停用词词典，用于token过滤，得到bag of words
  StopwordDict* stopword_dict_;

  // 词项token的idf信息，包括两类：commercial_idf 和 general_idf
  // 同时用于token和关键词的tfidf计算
  TokenIdfDict* token_idf_dict_;

  // WordEmbedding 词典
  WordEmbeddingDict* word_embedding_dict_;

  // 后续可以引入更多更丰富的词典

  DECLARE_UNCOPYABLE(DictManager);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_DICT_MANAGER_H_
