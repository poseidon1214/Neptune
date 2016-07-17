// Copyright (c) 2012 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
//  将 id:weight 格式的 features 文件转换为 string:weight 格式

#include <fstream>
#include <tr1/unordered_map>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/dict/vocabulary.h"

DEFINE_string(feature_vocabulary_file, "", "");
DEFINE_string(corpus_feature_file, "", "");
DEFINE_string(output_file, "", "");

namespace qzap {
namespace text_analysis {

bool Print() {
  Vocabulary vocabulary;
  CHECK(vocabulary.Load(FLAGS_feature_vocabulary_file));

  std::ifstream fin(FLAGS_corpus_feature_file.c_str());
  if (fin.fail()) {
    LOG(ERROR) << "test data file dose not exist";
    return false;
  }
  std::ofstream fout(FLAGS_output_file.c_str());
  if (fout.fail()) {
    LOG(ERROR) << "Failed to open the output_file: " << FLAGS_output_file;
    return false;
  }

  std::string line;
  while (std::getline(fin, line)) {
    TrimString(&line);
    std::vector<std::string> fields;
    SplitString(line, "\t", &fields);
    if (fields.size() > 2) {
      std::vector<std::string> features;
      SplitString(fields[1], " ", &features);
      std::string str = fields[0] + "\t";
      for (size_t i = 0; i < features.size(); ++i) {
        std::vector<std::string> feature_pair;
        SplitString(features[i], ":", &feature_pair);
        int value = -1;
        StringToNumeric(feature_pair[0], &value);
        StringAppendF(&str, "%s:%s ", vocabulary.Word(value).c_str(),
                      feature_pair[1].c_str());
      }
      StringAppendF(&str, "%s\n", fields[2].c_str());

      fout.write(str.c_str(), str.size());
    }
  }
  fin.close();
  fout.close();

  return true;
}

}  // namespace text_analysis
}  // namespace qzap

int main(int32_t argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);

  return qzap::text_analysis::Print();
}

