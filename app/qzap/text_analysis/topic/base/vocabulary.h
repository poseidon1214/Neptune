// Copyright 2011 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_VOCABULARY_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_VOCABULARY_H_

#include <stdint.h>
#include <fstream>
#include <string>
#include <tr1/unordered_map>
#include <utility>
#include <vector>

#include "app/qzap/common/base/scoped_ptr.h"
#include "thirdparty/glog/logging.h"

namespace qzap {
namespace text_analysis {
namespace base {

class Vocabulary {
 public:
  Vocabulary() {}

  explicit Vocabulary(const std::string& filepath) {
    CHECK(Load(filepath));
  }

  void Clear() {
    word_to_index_.clear();
    index_to_word_.clear();
  }

  // vocabulary size
  int32_t Size() const {
    return static_cast<int32_t>(index_to_word_.size());
  }

  // Load the vocabulary from a text file, whose each line could
  // contains either '\t'-delimited token_literal and token_frequency,
  // or only token_literal.  In either case, the (zero-based) line
  // numbers become token ids.  In the later case, all token
  // frequencies are considered zero.
  bool Load(std::ifstream* fin);

  // local file or hdfs file
  bool Load(const std::string& filepath);

  bool HasWord(const std::string& word) const {
    return word_to_index_.find(word) != word_to_index_.end();
  }

  // If |word| does not exist in the vocabulary, returns -1.
  int32_t WordIndex(const std::string& word) const {
    std::tr1::unordered_map<std::string, int32_t>::const_iterator iter =
        word_to_index_.find(word);
    if (iter != word_to_index_.end()) {
      return iter->second;
    }
    return -1;
  }

  // May cause segmentation fault if index is out of the range.
  const std::string& Word(int32_t index) const {
    CHECK_GE(index, 0);
    CHECK_LT(index, Size());
    return index_to_word_[index];
  }

  // This function is for unit test.
  void AddWord(const std::string& word) {
    CHECK(word_to_index_.find(word) == word_to_index_.end());
    index_to_word_.push_back(word);
    word_to_index_[word] = index_to_word_.size() - 1;
  }

 private:
  std::tr1::unordered_map<std::string, int32_t> word_to_index_;
  std::vector<std::string> index_to_word_;
};  // class Vocabulary

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_VOCABULARY_H_
