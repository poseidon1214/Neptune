// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// 关键词抽取类

#ifndef APP_QZAP_TEXT_ANALYSIS_KEYWORD_KEYWORD_EXTRACTOR_H_
#define APP_QZAP_TEXT_ANALYSIS_KEYWORD_KEYWORD_EXTRACTOR_H_

#include <string>
#include <tr1/unordered_map>
#include <vector>

#include "app/qzap/common/base/scoped_ptr.h"
#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/dict_manager.h"

namespace qzap {
namespace text_analysis {

class KeywordInfo;
class Keyword;
class Document;
class Field;

// Keyword 抽取的具体步骤如下：
//   1、对给定的输入文本进行分词、词性标注，过滤停用词，得到 tokens
//   2、多模式串匹配，抽取文本中包含在 kyeword 词典中的所有词条, 作为 candidates
//   3、计算 candidates(reranking weight) 的权重
class KeywordExtractor {
 public:
  // 构造函数ctor，必须使用 DictManager 对象初始化
  // dict_manager: 词典管理器，与 keyword extraction 模块解耦，当使用某词典时，
  // 只需要从 dict_manager 中 Get 相应词典指针即可
  explicit KeywordExtractor(const DictManager& dict_manager);
  ~KeywordExtractor() {};

  // 在分词结果基础上, 抽取Keyword, 构建Bag-Of-Keywords模型
  bool Extract(Document* document) const;

 private:
  typedef
      std::tr1::unordered_map<std::string, KeywordInfo> KeywordInfoMap;
  typedef
      std::tr1::unordered_map<std::string, Keyword*> BowKeywordsMap;
  typedef BowKeywordsMap::iterator BowKeywordsMapIter;

  bool ExtractDocumentKeywords(Document* document) const;

  // 检查 keyword_text 是否为有效 keyword
  bool IsValid(const std::string& keyword_text) const;

  void CalcWeight(Document* document) const;

  // 词典管理器
  const DictManager& dict_manager_;

  DECLARE_UNCOPYABLE(KeywordExtractor);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_KEYWORD_KEYWORD_EXTRACTOR_H_
