// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/token_idf_dict.h"

#include <string>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(commercial_file, "",
              "input commercial_idf file to token_idf");

DEFINE_string(general_file, "",
              "input general_idf path to token_idf");

DEFINE_string(output_file, "",
              "output filename of built binary of token_idf dictionary");

int main(int argc, char **argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);

  qzap::text_analysis::TokenIdfDict dict;
  if (!dict.Build(FLAGS_commercial_file, FLAGS_general_file)) {
    LOG(FATAL) << "Fail to build token_idf dictionary using commercial_file["
        << FLAGS_commercial_file << "] and general_file["
        << FLAGS_general_file << "].";
  }
  if (!dict.Save(FLAGS_output_file)) {
    LOG(FATAL) << "Fail to save token_idf dictionary binary to "
        << FLAGS_output_file;
  }

  return 0;
}
