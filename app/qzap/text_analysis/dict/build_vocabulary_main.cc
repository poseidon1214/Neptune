// Copyright 2013 Tencent Inc.
// Author: Zhihui JIN (rickjin@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/vocabulary.h"

#include <string>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(input_file, "",
              "the input file of vocabulary");

DEFINE_string(output_file, "",
              "the output file of built binary vocabulary");

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);

  qzap::text_analysis::Vocabulary vocabulary;
  if (!vocabulary.Build(FLAGS_input_file)) {
    LOG(FATAL) << "Fail to build vocabulary using " << FLAGS_input_file;
  }
  if (!vocabulary.Save(FLAGS_output_file)) {
    LOG(FATAL) << "Fail to save vocabulary binary to " << FLAGS_output_file;
  }

  return 0;
}
