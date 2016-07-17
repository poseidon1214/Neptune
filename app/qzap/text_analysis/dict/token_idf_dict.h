// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// Token Idf 词典类

#ifndef APP_QZAP_TEXT_ANALYSIS_TOKEN_IDF_DICT_H_
#define APP_QZAP_TEXT_ANALYSIS_TOKEN_IDF_DICT_H_

#include <map>
#include <string>
#include <vector>

#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/dict/dict.pb.h"
#include "app/qzap/text_analysis/dict/strkey_value_dict_base.h"

namespace qzap {
namespace text_analysis {

class TokenIdfDict : public StrKeyValueDictBase<IdfInfo> {
 public:
  TokenIdfDict();
  ~TokenIdfDict();

  bool Build(const std::string& commercial_file,
             const std::string& general_file);

 private:
  bool LoadIdf(const std::string& filename,
               std::map<std::string, float>* idf_map);
  void MergeIdfInfo(const std::map<std::string, float>& commercial_map,
                    const std::map<std::string, float>& general_map,
                    std::vector<const char*>* tokens,
                    std::vector<Darts::DoubleArray::value_type>* values);

 private:
  DECLARE_UNCOPYABLE(TokenIdfDict);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOKEN_IDF_DICT_H_
