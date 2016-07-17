// Copyright (c) 2011 Tencent Inc.
// Author: Pan Yang (baileyyang@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)
//
// Description: Feature Extractor based on TextMiner

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_FEATURE_FEATURE_EXTRACTOR_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_FEATURE_FEATURE_EXTRACTOR_H_

#include <map>
#include <string>

#include "app/qzap/common/base/scoped_ptr.h"
#include "common/base/uncopyable.h"

namespace qzap {
namespace text_analysis {

class Document;
class Vocabulary;
class Instance;

// 分类器特征抽取模块，每个类别的特征以数字编号加“-”开头，设计如下：
//   1. Token 特征: 以"1-"开头，后接token name, 如"1-iphone, 1-ipad";
//   2. Keyword 特征: 以"2-"开头，后接keyword name, 如"2-iphone 4s, 2-ipad 2";
//   3. Topic 特征: 以"3-"开头，后接topic id, 如"3-1, 3-10";
//   4. Embedding 特征: 以"4-"开头，后接embeding dimension, 如"4-1, 4-10";
//   5. 新增加的特征类别，可按如上方式命名并在此加以注释
//   ...
//
// TODO(fandywang): 特征值到id的映射不应该集成在FeatureExtractor中,
// 只需要在Predicter中实现映射即可
class FeatureExtractor {
 public:
  FeatureExtractor();
  virtual ~FeatureExtractor();

  // 加载feature词典
  bool LoadFeatureVocab(const std::string& feature_vocab);

  // 抽取BOW Token特征
  void ExtractTokenFeature(const Document& document, Instance* instance) const;

  // 抽取商业Keyword特征
  void ExtractKeywordFeature(const Document& document, Instance* instance) const;

  // 抽取Topic特征
  void ExtractTopicFeature(const Document& document, Instance* instance) const;

  // 抽取Embedding特征
  void ExtractEmbeddingFeature(const Document& document,
                               Instance* instance) const;

  // TODO(fandywang): 添加更多特征

 private:
  void AddFeature(const std::string& feature,
                  double weight,
                  Instance* instance) const;

  // 原始特征值到id的映射表, 基于training data构建
  scoped_ptr<Vocabulary> vocabulary_;

  DECLARE_UNCOPYABLE(FeatureExtractor);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_FEATURE_FEATURE_EXTRACTOR_H_
