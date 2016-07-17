// Copyright (c) 2014 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// 对文本做 topic inference，一般评估使用

#include <fstream>
#include <set>
#include <string>
#include <vector>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/text_miner.h"
#include "app/qzap/text_analysis/text_miner.pb.h"
#include "app/qzap/text_analysis/text_miner_resource.h"

DEFINE_string(input_file, "", "the input file");
DEFINE_string(peacock_ttw_file, "", "the topic top word file");

DECLARE_string(segmenter_data_dir);
DECLARE_string(text_miner_resource_config_file);

using namespace std;
using namespace qzap::text_analysis;

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);

  // 加载 peacock ttw 词典
  std::ifstream fs(FLAGS_peacock_ttw_file.c_str());
  if (fs.fail()) {
    LOG(ERROR) << "Open file " << FLAGS_peacock_ttw_file << " failed.";
    return false;
  }
  std::map<uint32_t, string> ttw;
  std::string line;
  while (std::getline(fs, line)) {
    std::vector<std::string> fds;
    SplitString(line, "\t", &fds);
    if (fds.size() != 2) {
      LOG(ERROR) << "Line format error, line: " << line;
      continue;
    }
    StringReplace(&fds[0], "topic ", "");
    size_t found = fds[0].find_first_of("(");
    fds[0] = fds[0].substr(0, found);
    uint32_t id;
    if (StringToNumeric(fds[0], &id)) {
      std::vector<std::string> words;
      SplitString(fds[1], " ", &words);
      std::string text;
      for (int i = 0; i < words.size() && i < 10; ++i) {
        text += words[i] + " ";
      }
      ttw[id] = text;
    }
  }
  fs.close();

  // 初始化 TextMiner
  scoped_ptr<TextMinerResource> text_miner_resource;
  scoped_ptr<TextMiner> text_miner;
  text_miner_resource.reset(new TextMinerResource());
  text_miner_resource->InitFromConfigFile(
      FLAGS_text_miner_resource_config_file);
  text_miner.reset(new TextMiner(text_miner_resource.get()));

  // 读取文本文件，每行一个doc，切词
  std::ifstream fin(FLAGS_input_file.c_str());
  if (fin.fail()) {
    LOG(ERROR) << "Open file " << FLAGS_input_file << " failed.";
    return false;
  }
  // 输出文件名是在输入文件名后加 .out
  std::ofstream fout((FLAGS_input_file + ".out").c_str());
  if (fout.fail()) {
    LOG(ERROR) << "Open file " << FLAGS_input_file << " failed.";
    return false;
  }

  while (std::getline(fin, line)) {
    TrimString(&line);

    Document document;
    Field* field = document.add_field();
    field->set_text(line);
    text_miner->InferTopics(&document);

    // LOG(ERROR) << document.Utf8DebugString();
    fout << line << std::endl;
    for (size_t i = 0; i < document.topic_size() && i < 10; ++i) {
      fout << "\t"
          << i << "\t"
          << document.topic(i).id() << "\t"
          << document.topic(i).weight() << "\t"
          << ttw[document.topic(i).id()]
          << std::endl;
    }
    fout << std::endl;
  }

  fin.close();
  fout.close();

  return 0;
}

