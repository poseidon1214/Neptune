// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/keyword_dict.h"

#include <string>
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(keyword_file, "", "keyword file");

DEFINE_string(output_file, "", "output file");

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);

  qzap::text_analysis::KeywordDict dict;
  if (!dict.Build(FLAGS_keyword_file.c_str())) {
    LOG(FATAL) << "Fail to build keyword dictionary from "
        << FLAGS_keyword_file;
  }

  if (!dict.Save(FLAGS_output_file.c_str())) {
    LOG(FATAL) << "Fail to save keyword dictionary binary to "
        << FLAGS_output_file;
  }
}
