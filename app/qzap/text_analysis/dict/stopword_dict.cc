// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include <string>

#include "app/qzap/text_analysis/dict/stopword_dict.h"

namespace qzap {
namespace text_analysis {

StopwordDict::StopwordDict() {}

StopwordDict::~StopwordDict() { Clear(); }

void StopwordDict::Clear() { stopword_dict_.Clear(); }

bool StopwordDict::LoadFromTextFile(const std::string& filename) {
  return stopword_dict_.Build(filename);
}

bool StopwordDict::Load(const std::string& filename) {
  return stopword_dict_.Load(filename);
}

bool StopwordDict::IsStopword(const std::string& word) const {
  // TODO(fandywang) : 根据编码范围，判断 word 是否为日文、韩文等字符串，
  // 以后会一并过滤这类词
  return stopword_dict_.HasWord(word);
}

}  // namespace text_analysis
}  // namespace qzap
