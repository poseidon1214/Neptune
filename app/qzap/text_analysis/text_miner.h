// Copyright (c) 2012 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// 文本分析工具类

#ifndef APP_QZAP_TEXT_ANALYSIS_TEXT_MINER_H_
#define APP_QZAP_TEXT_ANALYSIS_TEXT_MINER_H_

#include "thirdparty/gflags/gflags.h"

#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

namespace qzap {
namespace text_analysis {

class Document;  // 文档类, 包含详细的输入信息和处理结果
class TextMinerResource;  // 资源管理器

// TextMiner: 为情境广告系统提供一套统一的文本分析底层处理工具，供上层应用模块
// 调用，如User Analyzer, Page Server, CTR, Updater等，以保证文本分析相关的
// 概念、数据和代码的一致性，避免重复开发。
//
// TextMiner主要包括如下功能:
// 1. 输入：多个带权重的字段Document.fields,可以是页面内容、用户生成内容或者广告
// 2. 资源管理：TextMinerResource类负责，主要包括词典(如token_idf、keyword、
//    stopword等)和模型(如topic-model、classifier-model)两部分数据文件，对于
//    频繁更新的数据要兼容线上使用AIB管理，便于线上数据的实时更新
// 3. 词法分析：提供细粒度的中文分词和词性标注,处理结果写入Document.field.tokens
// 4. Token抽取：构建bag of words模型,抽取结果写入Document.bow_tokens
// 5. 通用Keyword抽取：抽取文本中包含的通用Keywords,处理结果写入
//    Document.field.tokens.bow_general_keywords,用于分类器和LDA模块
// 6. Keyword抽取：抽取与文本内容最相关且具有商业价值的词或短语,抽取结果写入
//    Document.bow_keywords
// 7. Keyword扩展：对抽取的Keyword进行语义扩展，写入Document.bow_keywords
// 8. Topic识别：对文本内容进行聚类分析，获取能够表达语义的文本主题,抽取结果写入
//    Documen.topics
// 9. TopicWord识别：对文本内容进行聚类分析，获取能够表达语义的文本主题词,
//    抽取结果写入Documen.topic_words
// 10. 文本分类：对文本内容进行分类分析，获取能够表达语义的行业类别,分类结果写入
//    Document.categories
// 11. Embedding：对文本内容进行表示学习，获取能够表达语义的向量表示,结果写入
//    Document.embedding
//
// 详细请参考：cocktail/html_doc/internal/content_analyzer/text_miner.html
class TextMiner {
 public:
  explicit TextMiner(TextMinerResource* text_miner_resource);
  ~TextMiner();

  // 词法分析(中文分词和词性标注)
  bool Segment(Document* document) const;

  // Token 抽取,构建 Bag-Of-Words tokens
  bool ExtractTokens(Document* document) const;

  // 商业Keyword抽取,构建 Bag-Of-Words keywords
  bool ExtractKeywords(Document* document) const;

  // Topic 识别,返回的dense形式的topic分布
  bool InferTopics(Document* document) const;

  // TopicWord 识别,返回的dense形式的topic word分布
  bool ExplainTopicWords(Document* document) const;

  // 分类
  bool Classify(Document* document) const;

  // Embedding 表示学习
  bool InferEmbedding(Document* document) const;

 private:
  // 资源管理器：包括数据和算法句柄
  TextMinerResource* text_miner_resource_;

  DECLARE_UNCOPYABLE(TextMiner);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TEXT_MINER_H_
