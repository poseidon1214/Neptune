// Copyright (c) 2014 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// 对文本做 bow keyword 抽取，处理结果保存到文件中

#include <fstream>
#include <set>
#include <string>
#include <vector>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/document_utils.h"
#include "app/qzap/text_analysis/text_miner.h"
#include "app/qzap/text_analysis/text_miner.pb.h"
#include "app/qzap/text_analysis/text_miner_resource.h"

DEFINE_string(input_file, "", "the input file");

DECLARE_string(segmenter_data_dir);
DECLARE_string(text_miner_resource_config_file);

using namespace std;
using namespace qzap::text_analysis;

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);

  // 初始化 TextMiner
  scoped_ptr<TextMinerResource> text_miner_resource;
  scoped_ptr<TextMiner> text_miner;
  text_miner_resource.reset(new TextMinerResource());
  text_miner_resource->InitFromConfigFile(
      FLAGS_text_miner_resource_config_file);
  text_miner.reset(new TextMiner(text_miner_resource.get()));

  // 读取文本文件，每行一个doc
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

  std::string line;
  while (std::getline(fin, line)) {
    TrimString(&line);

    Document document;
    Field* field = document.add_field();
    field->set_text(line);
    text_miner->ExtractKeywords(&document);

    fout << line << std::endl;
    DocumentUtils::TruncateRepeatedField(0.0,
                                         document.mutable_bow_keyword());
    for (size_t i = 0; i < document.bow_keyword_size(); ++i) {
      fout << "\t" << i << "\t" << document.bow_keyword(i).text()
          << "\t" << document.bow_keyword(i).weight() << std::endl;
    }
    fout << std::endl;
  }

  fin.close();
  fout.close();

  return 0;
}

