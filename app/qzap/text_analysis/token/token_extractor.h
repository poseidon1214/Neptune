// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// Token 抽取器: 构造 Bag-Of-Words 模型

#ifndef APP_QZAP_TEXT_ANALYSIS_TOKEN_TOKEN_EXTRACTOR_H_
#define APP_QZAP_TEXT_ANALYSIS_TOKEN_TOKEN_EXTRACTOR_H_

#include <stdint.h>
#include <string>
#include <tr1/unordered_map>

#include "common/base/uncopyable.h"

namespace qzap {
namespace text_analysis {

class DictManager;  // 词典管理器
class Document;  // 文档类
class Field;
class TokenOccurence;
class TokenIdfDict;
class Token;

// 在词法分析(中文分词和词性标注)基础上, 统计各Field下的Token, 构建
// Bag-Of-Words 模型
class TokenExtractor {
 public:
  explicit TokenExtractor(const DictManager& dict_manager);
  ~TokenExtractor();

  // 在分词和命名实体识别结果基础上, 抽取Token, 构建Bag-Of-Words模型
  // Note: 如果只需要重要的Token, 请设置FLAGS_extract_keytokens_only = true
  bool Extract(Document* document) const;

 private:
  typedef std::tr1::unordered_map<std::string, Token*> BowTokensMap;
  typedef BowTokensMap::iterator BowTokensMapIter;

  void AddBowToken(const Field& field,
                   const TokenOccurence& token,
                   const TokenIdfDict* token_idf_dict,
                   Document* document,
                   BowTokensMap* bow_tokens_map) const;

  // 获取word的加权Idf
  double GetIdf(const TokenIdfDict& token_idf_dict,
                const std::string& word) const;

  // 判断Token是否满足过滤条件: 停用词或者非功能词
  bool IsFilter(const TokenOccurence& token) const;

  // 词典管理器句柄, 主要使用 token_idf_dict
  const DictManager& dict_manager_;

  DECLARE_UNCOPYABLE(TokenExtractor);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOKEN_TOKEN_EXTRACTOR_H_

