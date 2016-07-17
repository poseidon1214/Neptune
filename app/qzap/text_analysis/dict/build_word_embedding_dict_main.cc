// Copyright 2015 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// word embedding 词典由文本格式编译成二进制格式

#include "app/qzap/text_analysis/dict/word_embedding_dict.h"

#include <string>
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(word_embedding_file, "", "word embedding file");

DEFINE_string(output_file, "", "output file");

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);

  qzap::text_analysis::WordEmbeddingDict dict;
  if (!dict.Build(FLAGS_word_embedding_file.c_str())) {
    LOG(FATAL) << "Fail to build word embedding dictionary from "
        << FLAGS_word_embedding_file;
  }

  if (!dict.Save(FLAGS_output_file.c_str())) {
    LOG(FATAL) << "Fail to save word embedding dictionary binary to "
        << FLAGS_output_file;
  }
}
