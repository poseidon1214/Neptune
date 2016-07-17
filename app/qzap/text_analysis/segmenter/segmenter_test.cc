// Copyright (c) 2011 Tencent Inc.
// Author: Huan Yu (huanyu@tencent.com)

#include "app/qzap/text_analysis/segmenter/segmenter.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/gtest/gtest.h"

using namespace std;
using namespace qzap::text_analysis;

TEST(SegmenterTest, Chinese) {
  Segmenter segmenter;
  vector<string> tokens;
  segmenter.Init();
  EXPECT_TRUE(segmenter.Segment("鲜花快递", &tokens));
  ASSERT_EQ(2U, tokens.size());
  EXPECT_EQ("鲜花", tokens[0]);
  EXPECT_EQ("快递", tokens[1]);
}

TEST(SegmenterTest, English) {
  Segmenter segmenter;
  vector<string> tokens;
  segmenter.Init();
  EXPECT_TRUE(segmenter.Segment("hello world", &tokens));
  ASSERT_EQ(3U, tokens.size());
  EXPECT_EQ("hello", tokens[0]);
  EXPECT_EQ(" ", tokens[1]);
  EXPECT_EQ("world", tokens[2]);
}

TEST(SegmenterTest, SinglePiece) {
  Segmenter segmenter;
  vector<string> tokens;
  segmenter.Init();
  EXPECT_TRUE(segmenter.Segment("helloworld", &tokens));
  ASSERT_EQ(1U, tokens.size());
  EXPECT_EQ("helloworld", tokens[0]);
}

TEST(SegmenterTest, CallMultipleTimes) {
  Segmenter segmenter;
  vector<string> tokens;
  segmenter.Init();
  EXPECT_TRUE(segmenter.Segment("hello world", &tokens));
  ASSERT_EQ(3U, tokens.size());
  EXPECT_EQ("hello", tokens[0]);
  EXPECT_EQ(" ", tokens[1]);
  EXPECT_EQ("world", tokens[2]);

  EXPECT_TRUE(segmenter.Segment("world hello", &tokens));
  ASSERT_EQ(3U, tokens.size());
  EXPECT_EQ("world", tokens[0]);
  EXPECT_EQ(" ", tokens[1]);
  EXPECT_EQ("hello", tokens[2]);
}

TEST(SegmenterTest, WordType) {
  Segmenter segmenter;
  vector<string> tokens;
  vector<string> word_types;
  segmenter.Init();
  EXPECT_TRUE(segmenter.SegmentWithWordType(
      "鲜花快递", &tokens, &word_types));
  ASSERT_EQ(2U, tokens.size());
  EXPECT_EQ("鲜花", tokens[0]);
  EXPECT_EQ("快递", tokens[1]);
  ASSERT_EQ(2U, word_types.size());
  EXPECT_EQ("n", word_types[0]);
  EXPECT_EQ("vn", word_types[1]);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  FLAGS_segmenter_data_dir = "data";
  return RUN_ALL_TESTS();
}

