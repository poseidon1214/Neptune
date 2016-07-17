// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// Keyword 词典类

#ifndef APP_QZAP_TEXT_ANALYSIS_KEYWORD_DICT_H_
#define APP_QZAP_TEXT_ANALYSIS_KEYWORD_DICT_H_

#include <string>
#include <vector>

#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/dict/dict.pb.h"
#include "app/qzap/text_analysis/dict/strkey_value_dict_base.h"

namespace qzap {
namespace text_analysis {

// Keyword 商业词典，主要由购买词 bidterm 组成，使用 darts 数据结构组织，
// 用于 keyword extraction 模块，匹配抽取所有包含在文本内容中词条
class KeywordDict : public StrKeyValueDictBase<KeywordInfo> {
 public:
  KeywordDict() {}
  ~KeywordDict() {}

  bool Build(const std::string& filename);

 private:
  DECLARE_UNCOPYABLE(KeywordDict);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_KEYWORD_DICT_H_
