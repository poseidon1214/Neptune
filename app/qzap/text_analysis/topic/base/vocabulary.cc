// Copyright 2011 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/base/vocabulary.h"

#include <fstream>
#include <functional>

#include "thirdparty/glog/logging.h"

#include "app/qzap/common/base/string_utility.h"

namespace qzap {
namespace text_analysis {
namespace base {

using std::string;
using std::vector;

bool Vocabulary::Load(std::ifstream* fin) {
  Clear();

  int32_t word_id = 0;  // Assign each word a 0-based index.
  std::string line;
  while (std::getline(*fin, line)) {
    TrimString(&line);
    if (line.length() > 0) {
      std::vector<std::string> field_vec;
      SplitString(line, "\t", &field_vec);
      if (word_to_index_.find(field_vec[0]) == word_to_index_.end()) {
        word_to_index_[field_vec[0]] = word_id;
        index_to_word_.push_back(field_vec[0]);
        ++word_id;
      }
    }
  }

  return true;
}

bool Vocabulary::Load(const std::string& filepath) {
  std::ifstream fin(filepath.c_str());
  bool flag = Load(&fin);
  fin.close();
  return flag;
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
