// Copyright (c) 2014 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// 关键词定向一体化 id 解析工具

#include <fstream>
#include <string>
#include <vector>

#include "thirdparty/gflags/gflags.h"

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/hash.h"
#include "app/qzap/service/targeting/common/keyword_id_alloc.h"

DEFINE_string(input_file, "", "the input file");

using namespace std;

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);

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
  fout << "# keyword_text\tsignature\tuser_keyword_id\tcontext_keyword_id"
      << std::endl;

  std::string line;
  while (std::getline(fin, line)) {
    TrimString(&line);
    std::vector<std::string> fds;
    SplitString(line, "\t", &fds);

    if (fds.size() == 0) { continue; }

    uint64_t signature = hash_string(fds[0]);
    uint64_t user_kw_id = qzap::targeting::GetKeywordTargetId(
        fds[0], qzap::targeting::kUser);
    uint64_t context_kw_id = qzap::targeting::GetKeywordTargetId(
        fds[0], qzap::targeting::kContext);

    fout << fds[0] << "\t"
        << signature << "\t"
        << user_kw_id << "\t"
        << context_kw_id << "\t"
        << int64_t(user_kw_id) << "\t"
        << int64_t(context_kw_id) << std::endl;
        //<< static_cast<int64_t>(user_kw_id) << "\t"
        //<< static_cast<int64_t>(context_kw_id) << std::endl;
  }

  for (int i = 0; i <= 10000; ++i) {
    uint64_t user_kw_id = qzap::targeting::GetTopicTargetId(
        i, qzap::targeting::kUser);
    uint64_t context_kw_id = qzap::targeting::GetTopicTargetId(
        i, qzap::targeting::kContext);

    fout << i << "\t"
        << i << "\t"
        << user_kw_id << "\t"
        << context_kw_id << "\t"
        << int64_t(user_kw_id) << "\t"
        << int64_t(context_kw_id) << std::endl;
  }

  fin.close();
  fout.close();

  return 0;
}

