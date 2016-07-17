// Copyright (c) 2014 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//         Yafei Zhang (kimmyzhang@tencent.com)
//
// 对文本行分词

#include <fstream>  // NOLINT
#include <iostream>  // NOLINT
#include <set>
#include <string>
#include <vector>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/segmenter/segmenter.h"

DEFINE_string(separator, "\t", "Output separator.");
DEFINE_string(input_file, "-",
              "The input file. \"-\" or empty denotes the standard input.");
DEFINE_string(output_file, "-",
              "The output file. \"-\" or empty denotes the standard output.");
DEFINE_string(stopword_file, "", "The stopword file.");
DEFINE_bool(word_type, false, "Enable printing word type.");

using namespace qzap::text_analysis;  // NOLINT

bool LoadStopWord(const std::string& filename,
                  std::set<std::string>* stopwords) {
  // 加载停用词词典
  std::ifstream fs(filename.c_str());
  if (fs.fail()) {
    return false;
  }

  std::string line;
  while (std::getline(fs, line)) {
    stopwords->insert(line);
  }
  fs.close();
  return true;
}

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_separator.empty()) {
    LOG(ERROR) << "Separator must be not empty.";
    return 1;
  }

  std::istream* fin;
  if (FLAGS_input_file == "-" || FLAGS_input_file.empty()) {
    fin = &std::cin;
  } else {
    fin = new std::ifstream(FLAGS_input_file.c_str());
    if (fin->fail()) {
      LOG(ERROR) << "Open file " << FLAGS_input_file << " failed.";
      return 2;
    }
  }

  std::ostream* fout;
  if (FLAGS_output_file == "-" || FLAGS_output_file.empty()) {
    fout = &std::cout;
  } else {
    fout = new std::ofstream(FLAGS_output_file.c_str());
    if (fout->fail()) {
      LOG(ERROR) << "Open file " << FLAGS_output_file << " failed.";
      return 3;
    }
  }

  std::set<std::string> stopwords;
  if (!FLAGS_stopword_file.empty()) {
    if (!LoadStopWord(FLAGS_stopword_file, &stopwords)) {
      LOG(ERROR) << "Load stop word file " << FLAGS_stopword_file << " failed.";
      return 4;
    }
  }

  // 初始化分词器
  Segmenter segmenter;
  segmenter.Init();

  // 读取fin, 每行一个doc, 切词, 输出到fout
  std::string line;
  std::vector<std::string> tokens;
  std::vector<std::string> word_types;
  std::vector<std::string>* p_word_types = NULL;
  if (FLAGS_word_type) {
    p_word_types = &word_types;
  }

  while (std::getline(*fin, line)) {
    TrimString(&line);

    tokens.clear();
    if (p_word_types) {
      word_types.clear();
    }
    if (!segmenter.SegmentWithWordType(line, &tokens, p_word_types)) {
      LOG(ERROR) << "Segment \"" << line << "\" failed.";
      continue;
    }

    std::string text;
    for (size_t i = 0; i < tokens.size(); ++i) {
      if (!stopwords.empty() && stopwords.count(tokens[i]) != 0) {
        continue;
      }

      text += tokens[i];
      if (p_word_types) {
        text += "(" + word_types[i] + ")";
      }
      text += FLAGS_separator;
    }

    if (!text.empty()) {
      text.resize(text.size() - FLAGS_separator.size());
      *fout << text << std::endl;
    }
  }

  if (fin != &std::cin) {
    delete fin;
  }
  if (fout != &std::cout) {
    delete fout;
  }

  return 0;
}
