// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/base/hyperparams.h"

#include <fstream>

#include "thirdparty/gtest/gtest.h"

using qzap::text_analysis::base::Hyperparams;
using qzap::text_analysis::base::HyperparamsPB;

const std::string kSSTableFilename = "hyperparams";
const int32_t kNumTopics = 6;
const int32_t kNumWords = 1600;

TEST(HyperparamsTest, CtorGetter) {
  // special case
  Hyperparams param(0.1, 1, 0.01, 1);
  EXPECT_EQ(param.NumTopics(), 1);
  EXPECT_EQ(param.VocabSize(), 1);

  EXPECT_DOUBLE_EQ(param.WordPrior(), 0.01);
  EXPECT_DOUBLE_EQ(param.WordPriorSum(), 0.01);
  for (int32_t i = 0; i < 1; ++i) {
    EXPECT_DOUBLE_EQ(param.TopicPrior(i), 0.1);
  }
  EXPECT_DOUBLE_EQ(param.TopicPriorSum(), 0.1);

  // general case
  Hyperparams param1(0.1, kNumTopics, 0.01, kNumWords);
  EXPECT_EQ(kNumTopics, param1.NumTopics());
  EXPECT_EQ(kNumWords, param1.VocabSize());

  EXPECT_DOUBLE_EQ(param1.WordPrior(), 0.01);
  EXPECT_DOUBLE_EQ(param1.WordPriorSum(), 0.01 * kNumWords);
  for (int32_t i = 0; i < kNumTopics; ++i) {
    EXPECT_DOUBLE_EQ(param1.TopicPrior(i), 0.1);
  }
  EXPECT_DOUBLE_EQ(param1.TopicPriorSum(), 0.1 * kNumTopics);
}

TEST(HyperparamsTest, SerializeParse) {
  HyperparamsPB pb;
  pb.set_word_prior(0.01);
  for (int32_t i = 0; i < kNumTopics; ++i) {
    pb.add_topic_prior(0.1);
  }
  pb.set_vocab_size(kNumWords);

  Hyperparams param(0.1, kNumTopics, 0.01, kNumWords);
  HyperparamsPB pb2;
  param.SerializeTo(&pb2);

  EXPECT_EQ(pb.DebugString(), pb2.DebugString());

  std::ofstream fout(kSSTableFilename.c_str());
  EXPECT_TRUE(param.Save(&fout));
  fout.close();

  Hyperparams param2(kSSTableFilename);
  param2.SerializeTo(&pb2);

  EXPECT_EQ(pb.DebugString(), pb2.DebugString());

  std::ifstream fin(kSSTableFilename.c_str());
  EXPECT_TRUE(param2.Load(&fin));
  fin.close();
  param2.SerializeTo(&pb2);

  EXPECT_EQ(pb.DebugString(), pb2.DebugString());

  Hyperparams param3(0.1, 1, 0.01, 1);
  param3.ParseFrom(pb);
  param3.SerializeTo(&pb2);

  EXPECT_EQ(pb.DebugString(), pb2.DebugString());
}
