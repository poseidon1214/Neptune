// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/training/train_classifier_base.h"

#include "thirdparty/gtest/gtest.h"

namespace qzap {
namespace text_analysis {

using std::string;
using std::vector;

TEST(TrainClassifierBaseTest, ParseTrainSampleStringErrorSeperator) {
  vector<string> category;
  Instance feature;
  string sample_str;
  string comments;

  sample_str = "游戏--小游戏--QQ小游戏;0:0.1 10:0.9;我喜欢玩游戏";
  EXPECT_FALSE(TrainClassifierBase::ParseTrainSampleString(sample_str,
                                                           &category,
                                                           &feature,
                                                           &comments));
}

TEST(TrainClassifierBaseTest, ParseTrainSampleStringEmptyCategoryInput) {
  vector<string> category;
  Instance feature;
  string sample_str;
  string comments;

  sample_str = "\t0:0.1 10:0.9\t我喜欢玩游戏";
  EXPECT_FALSE(TrainClassifierBase::ParseTrainSampleString(sample_str,
                                                           &category,
                                                           &feature,
                                                           &comments));

  sample_str = "\t\t";
  EXPECT_FALSE(TrainClassifierBase::ParseTrainSampleString(sample_str,
                                                           &category,
                                                           &feature,
                                                           &comments));
}

TEST(TrainClassifierBaseTest, ParseTrainSampleStringEmptyFeatureInput) {
  vector<string> category;
  Instance feature;
  string sample_str;
  string comments;

  sample_str = "游戏--小游戏--QQ小游戏\t\t";
  EXPECT_TRUE(TrainClassifierBase::ParseTrainSampleString(sample_str,
                                                          &category,
                                                          &feature,
                                                          &comments));
  ASSERT_EQ(3u, category.size());
  EXPECT_EQ("游戏", category[0]);
  EXPECT_EQ("游戏--小游戏", category[1]);
  EXPECT_EQ("游戏--小游戏--QQ小游戏", category[2]);
  EXPECT_EQ(0u, feature.NumFeatures());
  EXPECT_EQ("", comments);
}

TEST(TrainClassifierBaseTest, ParseTrainSampleStringRegular) {
  vector<string> category;
  Instance feature;
  string sample_str;
  string comments;

  sample_str = "游戏--小游戏--QQ小游戏\t0:0.1 10:0.9\t...";
  EXPECT_TRUE(TrainClassifierBase::ParseTrainSampleString(sample_str,
                                                          &category,
                                                          &feature,
                                                          &comments));
  ASSERT_EQ(3u, category.size());
  EXPECT_EQ("游戏", category[0]);
  EXPECT_EQ("游戏--小游戏", category[1]);
  EXPECT_EQ("游戏--小游戏--QQ小游戏", category[2]);

  ASSERT_EQ(2u, feature.NumFeatures());
  feature.SortById();
  EXPECT_EQ(0u, feature.IdAt(0));
  EXPECT_DOUBLE_EQ(0.1, feature.WeightAt(0));
  EXPECT_EQ(10u, feature.IdAt(1));
  EXPECT_DOUBLE_EQ(0.9, feature.WeightAt(1));

  EXPECT_EQ("...", comments);
}

}  // namespace text_analysis
}  // namespace qzap

