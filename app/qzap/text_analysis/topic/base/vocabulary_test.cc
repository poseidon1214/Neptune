// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include <string>

#include "thirdparty/gtest/gtest.h"

#include "app/qzap/text_analysis/topic/base/vocabulary.h"

using std::pair;
using std::string;
using std::vector;
using qzap::text_analysis::base::Vocabulary;

TEST(Vocabulary, AllFunctions) {
  const string vocab_path = "testdata/vocab";
  Vocabulary vocab(vocab_path);
  EXPECT_EQ(11, vocab.Size());

  EXPECT_EQ(10, vocab.WordIndex("tiger"));
  EXPECT_EQ(-1, vocab.WordIndex("whatever"));
  EXPECT_EQ(vocab.Word(9), "cat");
}

