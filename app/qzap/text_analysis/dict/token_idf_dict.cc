// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/token_idf_dict.h"

#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"

namespace qzap {
namespace text_analysis {

TokenIdfDict::TokenIdfDict():StrKeyValueDictBase<IdfInfo>() {}

TokenIdfDict::~TokenIdfDict() { Clear(); }

bool TokenIdfDict::Build(const std::string& commercial_file,
                         const std::string& general_file) {
  Clear();
  std::map<std::string, float> commercial_map;
  std::map<std::string, float> general_map;
  if (!LoadIdf(commercial_file, &commercial_map)) {
    return false;
  }
  if (!LoadIdf(general_file, &general_map)) {
    return false;
  }

  std::vector<const char*> tokens;
  std::vector<Darts::DoubleArray::value_type> values;
  MergeIdfInfo(commercial_map, general_map, &tokens, &values);
  int ret = (dict_.build(tokens.size(), &tokens[0], NULL, &values[0]));
  return ret == 0;
}

bool TokenIdfDict::LoadIdf(const std::string& filename,
                           std::map<std::string, float>* idf_map) {
  std::string line;
  std::ifstream fin(filename.c_str());
  if (!fin) {
    LOG(ERROR) << "open file '" << filename << "' failed.";
    return false;
  }

  while (getline(fin, line)) {
    TrimString(&line);
    if (line.length() == 0) { continue; }
    std::vector<std::string> vec;
    float idf;
    SplitString(line, "\t", &vec);
    if (vec.size() == 2 && StringToNumeric(vec[1], &idf)
        && vec[0].length() > 0) {
      idf_map->insert(std::pair<std::string, float>(vec[0], idf));
    }
  }
  fin.close();

  return true;
}

void TokenIdfDict::MergeIdfInfo(
    const std::map<std::string, float>& commercial_map,
    const std::map<std::string, float>& general_map,
    std::vector<const char*>* tokens,
    std::vector<Darts::DoubleArray::value_type>* values) {
  std::map<std::string, float>::const_iterator commercial_iter =
      commercial_map.begin();
  std::map<std::string, float>::const_iterator general_iter =
      general_map.begin();
  while (commercial_iter != commercial_map.end() &&
         general_iter != general_map.end()) {
    IdfInfo idf_info;
    int compare_result = commercial_iter->first.compare(general_iter->first);
    bool flag = false;
    if (compare_result == 0) {
      tokens->push_back(commercial_iter->first.c_str());
      idf_info.set_idf_commercial(commercial_iter->second);
      idf_info.set_idf_general(general_iter->second);
      ++commercial_iter;
      ++general_iter;
      flag = true;
    } else if (compare_result < 0) {
      if (strlen(commercial_iter->first.c_str()) > 0) {
        tokens->push_back(commercial_iter->first.c_str());
        idf_info.set_idf_commercial(commercial_iter->second);
        flag = true;
      }
      ++commercial_iter;
    } else {
      tokens->push_back(general_iter->first.c_str());
      idf_info.set_idf_general(general_iter->second);
      ++general_iter;
      flag = true;
    }
    if (flag) {
      values->push_back(tokens->size() - 1);
      value_vector_.push_back(idf_info);
    }
  }

  while (commercial_iter != commercial_map.end()) {
    IdfInfo idf_info;
    tokens->push_back(commercial_iter->first.c_str());
    idf_info.set_idf_commercial(commercial_iter->second);
    ++commercial_iter;
    values->push_back(tokens->size() - 1);
    value_vector_.push_back(idf_info);
  }
  while (general_iter != general_map.end()) {
    IdfInfo idf_info;
    tokens->push_back(general_iter->first.c_str());
    idf_info.set_idf_general(general_iter->second);
    ++general_iter;
    values->push_back(tokens->size() - 1);
    value_vector_.push_back(idf_info);
  }
}

}  // namespace text_analysis
}  // namespace qzap
