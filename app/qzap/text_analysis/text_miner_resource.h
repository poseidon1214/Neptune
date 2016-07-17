// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// 文本分析资源管理类

#ifndef APP_QZAP_TEXT_ANALYSIS_TEXT_MINER_RESOURCE_H_
#define APP_QZAP_TEXT_ANALYSIS_TEXT_MINER_RESOURCE_H_

#include <tr1/memory>
#include <tr1/unordered_map>

#include <string>

#include "thirdparty/gflags/gflags.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "common/base/singleton.h"
#include "common/base/uncopyable.h"
#include "app/qzap/common/thread/mutex.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

DECLARE_string(text_miner_resource_config_file);

DECLARE_string(segmenter_data_dir);

DECLARE_bool(extract_keytokens_only);  // default = true

DECLARE_int32(peacock_cache_size_mb);  // default = 5 * 1024
DECLARE_int32(peacock_num_markov_chains);  // default = 5
DECLARE_int32(peacock_total_iterations);  // default = 15
DECLARE_int32(peacock_burn_in_iterations);  // default = 10
DECLARE_int32(peacock_topic_top_k);
DECLARE_int32(peacock_topic_word_top_k);

DECLARE_double(hierarchical_classifier_threshold);  // default = 0.2
DECLARE_double(classifier_threshold);  // default = 0.1

namespace qzap {
namespace text_analysis {

class ResourceConfig;
class DictManager;  // 词典管理
class Segmenter;  // 分词器
class TokenExtractor;  // Token 抽取
class KeywordExtractor;  // 商业 Keyword 抽取
class TopicInferenceEngine;  // Topic 识别
class Classifier;  // 文本分类
class EmbeddingInferenceEngine;  // 表示学习

// 统一组织、管理文本分析工具 TextMiner 所使用的数据文件和算法模块句柄
class TextMinerResource {
 public:
  TextMinerResource();
  explicit TextMinerResource(const std::string& config_file);
  ~TextMinerResource();

  bool InitFromConfigFile(const std::string& config_file);
  bool InitFromPbMessage(const ResourceConfig& resource_config);

  // 显式释放资源
  void Clear();

  const std::string GetResourceName() const {
    return resource_config_.resource_name();
  }

  void SetResourceName(const std::string& resource_name) {
    resource_config_.set_resource_name(resource_name);
  }

  // 获取词典资源
  const DictManager* GetDictManager();

  // 获取分词器句柄
  const Segmenter* GetSegmenter();

  // 获取 Token 抽取器句柄
  const TokenExtractor* GetTokenExtractor();

  // 获取商业 Keyword 抽取器句柄
  const KeywordExtractor* GetKeywordExtractor();

  // 获取 topic inference engine 句柄
  const TopicInferenceEngine* GetTopicInferenceEngine();

  // 根据 resource_name 获取 classifier 句柄
  const Classifier* GetClassifier();

  // 获取 embedding inference engine 句柄
  const EmbeddingInferenceEngine* GetEmbeddingInferenceEngine();

 private:
  bool InitDictManager();
  bool InitSegmenter();
  bool InitTokenExtractor();
  bool InitKeywordExtractor();
  bool InitTopicInferenceEngine();
  bool InitClassifier();
  bool InitEmbeddingInferenceEngine();

  ResourceConfig resource_config_;

  // 词典资源管理器
  std::tr1::shared_ptr<DictManager> dict_manager_;
  bool is_init_dict_manager_;

  // 分词器
  scoped_ptr<Segmenter> segmenter_;
  bool is_init_segmenter_;

  // Token 抽取器
  scoped_ptr<TokenExtractor> token_extractor_;
  bool is_init_token_extractor_;

  // 商业 Keyword 抽取器
  scoped_ptr<KeywordExtractor> keyword_extractor_;
  bool is_init_keyword_extractor_;

  // Topic Inference Engine
  scoped_ptr<TopicInferenceEngine> topic_inference_engine_;
  bool is_init_topic_inference_engine_;

  // Classifier
  typedef std::tr1::unordered_map<std::string, Classifier*> ClassifierMap;
  typedef ClassifierMap::iterator ClassifierMapIter;
  ClassifierMap classifier_map_;

  // EmbeddingInferenceEngine
  scoped_ptr<EmbeddingInferenceEngine> embedding_inference_engine_;
  bool is_init_embedding_inference_engine_;

  Mutex dict_manager_mutex_;  // GUARDS GetDictManager()
  Mutex module_mutex_;  // GUARDS GetXX algorithm functions

  DECLARE_UNCOPYABLE(TextMinerResource);
};

// 单体模式, 保证内存中仅加载一份数据
typedef gdt::Singleton<TextMinerResource> SingletonTextMinerResource;

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TEXT_MINER_RESOURCE_H_

