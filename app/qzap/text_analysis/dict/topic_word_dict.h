// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// TopicWord 词典类: 提供 topic_id 到 topk words 的映射

#ifndef APP_QZAP_TEXT_ANALYSIS_DICT_TOPIC_WORD_DICT_H_
#define APP_QZAP_TEXT_ANALYSIS_DICT_TOPIC_WORD_DICT_H_

#include "app/qzap/common/base/uncopyable.h"
#include "app/qzap/text_analysis/dict/vocabulary.h"

namespace qzap {
namespace text_analysis {

// TopicWord 词典类, 基于 Vocabulary 词典对象采用组合的方式封装对外接口
class TopicWordDict {
 public:
  TopicWordDict();
  ~TopicWordDict();

  void Clear();

  // 加载二进制词典文件
  // Note: 使用 build_vocabulary.cc 离线编译得到二进制词典
  bool Load(const std::string& filename);
  // 直接加载纯文本文件, 格式为: topic_words \t topic_id
  bool LoadFromTextFile(const std::string& filename);

  // 获取 topic_id 对应的 Top-10 words
  bool GetTopicWords(uint32_t topic_id, std::string* words) const;
  // 获取 topic_id 对应的 Top-K words
  bool GetTopKTopicWords(uint32_t topic_id, uint32_t topk,
                         std::string* words) const;

 private:
  Vocabulary topic_word_dict_;

  DECLARE_UNCOPYABLE(TopicWordDict);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_DICT_TOPIC_WORD_DICT_H_

