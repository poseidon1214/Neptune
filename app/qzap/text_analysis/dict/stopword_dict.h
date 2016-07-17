// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// 停用词词典类

#ifndef APP_QZAP_TEXT_ANALYSIS_DICT_STOPWORD_DICT_H_
#define APP_QZAP_TEXT_ANALYSIS_DICT_STOPWORD_DICT_H_

#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/dict/vocabulary.h"

namespace qzap {
namespace text_analysis {

// 停用词词典类，基于 Vocabulary 词典对象采用组合的方式封装对外接口
// 注意：需要使用 build_vocabulary.cc 离线编译二进制词典文件
class StopwordDict {
 public:
  StopwordDict();
  ~StopwordDict();

  void Clear();

  // 加载词典，Load 函数与 AIB 保持兼容
  bool Load(const std::string& filename);

  bool LoadFromTextFile(const std::string& filename);

  bool IsStopword(const std::string& word) const;

 private:
  Vocabulary stopword_dict_;

  DECLARE_UNCOPYABLE(StopwordDict);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_DICT_STOPWORD_DICT_H_
