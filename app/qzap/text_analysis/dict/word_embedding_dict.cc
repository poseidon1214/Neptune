// Copyright 2015 Tencent Inc.
// Author: Guangneng Hu (lesliehu@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/word_embedding_dict.h"

#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"

namespace qzap {
namespace text_analysis {

bool WordEmbeddingDict::Build(const std::string& filename) {
  Clear();

  std::ifstream fin(filename.c_str());
  if (!fin) {
    LOG(ERROR) << "Open file '" << filename << "' failed.";
    return false;
  }

  std::vector<std::string> fields;
  std::string line;
  int vocab_size = -1;
  int dim = -1;

  if (getline(fin, line)) { // the first line is <vocab_size, embedding_dim>
    TrimString(&line);
    if (line.length() != 0) {
      SplitString(line, " ", &fields);
      if (fields.size() == 2
          && StringToNumeric(fields[0], &vocab_size)
          && StringToNumeric(fields[1], &dim)) {
        LOG(INFO) << "vocab_size: " << vocab_size
            << ", embedding_dim: " << dim;
      }
    }
  }

  if (vocab_size == -1 || dim == -1) {
    LOG(ERROR) << "Meta-data <vocab_size, embedding_dim> illegal.";
    return false;
  }

  std::map<std::string, Darts::DoubleArray::value_type> key_idx_map;
  while (getline(fin, line)) {
    TrimString(&line);
    if (line.length() == 0) { continue; }

    fields.clear();
    SplitString(line, " ", &fields);
    if (fields.size() == dim + 1 && fields[0].length() > 0) {
      EmbeddingInfo embedding_info;
      for (size_t i = 1; i < fields.size(); ++i) {
        float element;
        if (StringToNumeric(fields[i], &element)) {
          embedding_info.add_embedding(element);
        } else {
          LOG(ERROR) << "Format error, line : " << line;
          return false;
        }
      }
      value_vector_.push_back(embedding_info);
      key_idx_map[fields[0]] = value_vector_.size() - 1;
    }
  }
  fin.close();

  std::vector<const char*> key_vec;
  std::vector<Darts::DoubleArray::value_type> idx_vec;
  for (std::map<std::string, Darts::DoubleArray::value_type>::iterator it
      = key_idx_map.begin(); it != key_idx_map.end(); ++it) {
    key_vec.push_back(it->first.c_str());
    idx_vec.push_back(it->second);
  }

  if (key_vec.size() > 0) {
    int ret = dict_.build(key_vec.size(), &key_vec[0], NULL, &idx_vec[0]);
    if (ret != 0) {
      LOG(ERROR) << "Build word embedding Double-Array failed!";
      Clear();
      return false;
    }
  }

  return true;
}

}  // namespace text_analysis
}  // namespace qzap
