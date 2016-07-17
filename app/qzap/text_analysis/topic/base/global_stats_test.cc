// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/base/global_stats.h"

#include <fstream>

#include "thirdparty/gtest/gtest.h"

using qzap::text_analysis::base::GlobalStats;
using qzap::text_analysis::base::DenseTopicHistogram;

const char* kGlobalTopicHistogramFile = "global_topic_histogram.sstable";

TEST(GlobalStatsTest, ParseGetter) {
  GlobalStats g;
  EXPECT_EQ(g.NumTopics(), 0);

  DenseTopicHistogram histogram;
  histogram.Resize(6);
  histogram[4] = 5;
  histogram[2] = 1;
  histogram[0] = 2;
  g.ParseFrom(histogram);
  EXPECT_EQ(g.NumTopics(), 6);

  const DenseTopicHistogram& histogram2 = g.GetTopicHistogram();
  EXPECT_EQ(histogram2.NumTopics(), 6);
  for(int32_t i = 0; i < 6; ++i) {
    EXPECT_EQ(histogram[i], histogram2[i]);
  }

  DenseTopicHistogram& histogram3 = g.GetTopicHistogram();
  EXPECT_EQ(histogram3.NumTopics(), 6);
  for(int32_t i = 0; i < 6; ++i) {
    EXPECT_EQ(histogram[i], histogram3[i]);
  }
}

TEST(GlobalStatsTest, SaveLoad) {
  DenseTopicHistogram histogram;
  histogram.Resize(6);
  histogram[4] = 5;
  histogram[2] = 1;
  histogram[0] = 2;

  GlobalStats g;
  g.ParseFrom(histogram);
  std::ofstream fout(kGlobalTopicHistogramFile);
  EXPECT_TRUE(g.Save(&fout));
  fout.close();

  GlobalStats g2;
  std::ifstream fin(kGlobalTopicHistogramFile);
  EXPECT_TRUE(g2.Load(&fin));
  fin.close();

  EXPECT_EQ(g2.NumTopics(), 6);
  const DenseTopicHistogram& histogram2 = g2.GetTopicHistogram();
  for(int32_t i = 0; i < 6; ++i) {
    EXPECT_EQ(histogram[i], histogram2[i]);
  }
}

TEST(GlobalStatsTest, AppendAsString) {
  DenseTopicHistogram histogram;
  histogram.Resize(6);
  histogram[4] = 5;
  histogram[2] = 1;
  histogram[0] = 2;

  std::string s;
  histogram.AppendAsString(&s);
  EXPECT_EQ("2 0 1 0 5 0 ", s);
}
