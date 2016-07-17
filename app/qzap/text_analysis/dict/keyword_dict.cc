// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/keyword_dict.h"

#include <stdlib.h>
#include <stdint.h>
#include <fstream>
#include <map>

#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/dict/dict_io.h"

namespace qzap {
namespace text_analysis {

bool KeywordDict::Build(const std::string& filename) {
  Clear();

  std::ifstream fs;
  fs.open(filename.c_str(), std::ios_base::in);
  if (fs.fail()) {
    LOG(ERROR) << "Open file " << filename << " failed.";
    return false;
  }

  std::map<std::string, Darts::DoubleArray::value_type> key_idx_map;
  std::string line;
  while (std::getline(fs, line)) {
    TrimString(&line);
    if (line.length() > 0) {
      std::vector<std::string> field_vec;
      SplitString(line, "\t", &field_vec);
      KeywordInfo keyword_info;
      float weight = 0.0;
      if (field_vec.size() > 1 && StringToNumeric(field_vec[1], &weight)) {
        keyword_info.set_weight(weight);
      } else {
        keyword_info.set_weight(0.0);
      }
      value_vector_.push_back(keyword_info);
      key_idx_map[field_vec[0]] = value_vector_.size() - 1;
    }
  }
  fs.close();

  std::vector<const char*> key_vec;
  std::vector<Darts::DoubleArray::value_type> idx_vec;
  for (std::map<std::string, Darts::DoubleArray::value_type>::iterator it
      = key_idx_map.begin(); it != key_idx_map.end(); ++it) {
    key_vec.push_back(it->first.c_str());
    idx_vec.push_back(it->second);
  }

  if (key_vec.size() > 0) {
    int ret = dict_.build(
        key_vec.size(), &key_vec[0], NULL, &idx_vec[0]);
    if (ret != 0) {
      LOG(ERROR) << "Build keyword Double-Array failed!";
      Clear();
      return false;
    }
  }

  return true;
}

}  // namespace text_analysis
}  // namespace qzap
