// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/topic_word_dict.h"

#include <string>

#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"

namespace qzap {
namespace text_analysis {

TopicWordDict::TopicWordDict() {}

TopicWordDict::~TopicWordDict() { Clear(); }

void TopicWordDict::Clear() { topic_word_dict_.Clear(); }

bool TopicWordDict::LoadFromTextFile(const std::string& filename) {
  return topic_word_dict_.Build(filename);
}

bool TopicWordDict::Load(const std::string& filename) {
  return topic_word_dict_.Load(filename);
}

bool TopicWordDict::GetTopicWords(uint32_t topic_id, std::string* words) const {
  return GetTopKTopicWords(topic_id, 10, words);
}

bool TopicWordDict::GetTopKTopicWords(uint32_t topic_id,
                                      uint32_t topk,
                                      std::string* words) const {
  *words = "";
  std::string text = topic_word_dict_.Word(topic_id);
  if (text.empty()) {
    LOG(WARNING) << "the parameter topic_id '" << topic_id << "' is illegal.";
    return false;
  }

  std::vector<std::string> fields_vec;
  SplitString(text, ", ", &fields_vec);
  if (fields_vec.size() > 0 && topk > 0) {
    StringAppendF(words, "{");
    size_t len = 0;
    for (size_t i = 0; i < fields_vec.size() && i < topk; ++i) {
      len += fields_vec[i].length() + 2;
    }
    StringAppendF(words, "%s", text.substr(0, len - 2).c_str());
    StringAppendF(words, "}");
  }

  return true;
}

}  // namespace text_analysis
}  // namespace qzap

