// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/base/dense_topic_histogram.h"

#include <vector>

#include "thirdparty/gtest/gtest.h"

using qzap::text_analysis::base::DenseTopicHistogram;
using qzap::text_analysis::base::DenseTopicHistogramPB;
using std::vector;

const int32_t kNumTopics = 10;

void Equal(const DenseTopicHistogram& histogram, const vector<int32_t>& vec) {
  ASSERT_EQ(histogram.NumTopics(), static_cast<int32_t>(vec.size()));
  for (size_t i = 0; i < vec.size(); ++i) {
    EXPECT_EQ(histogram[i], vec[i]);
  }
}

TEST(DenseTopicHistogramTest, ResizeOperator) {
  DenseTopicHistogram histogram;
  vector<int32_t> vec;
  Equal(histogram, vec);

  histogram.Resize(kNumTopics);
  vec.resize(kNumTopics);
  Equal(histogram, vec);

  int32_t i = 4;
  ++histogram[i];
  ++vec[i];
  histogram[i + 1] = 6;
  vec[i + 1] = 6;
  Equal(histogram, vec);

  histogram.Resize(kNumTopics + 1);
  vec.assign(kNumTopics + 1, 0);
  Equal(histogram, vec);

  ++histogram[i];
  ++vec[i];

  histogram.Resize(kNumTopics - 1);
  vec.assign(kNumTopics - 1, 0);
  Equal(histogram, vec);
}

TEST(DenseTopicHistogramTest, SerializeParse) {
  int32_t i = 4;

  DenseTopicHistogram histogram;
  histogram.Resize(kNumTopics);
  ++histogram[i];
  histogram[i + 1] = 6;

  DenseTopicHistogramPB pb;
  for (int32_t j = 0; j < kNumTopics; ++j) {
    pb.add_count(0);
  }
  pb.set_count(i, 1);
  pb.set_count(i + 1, 6);

  DenseTopicHistogramPB pb2;
  histogram.SerializeTo(&pb2);
  EXPECT_EQ(pb2.DebugString(), pb.DebugString());

  histogram.ParseFrom(pb);
  histogram.SerializeTo(&pb);
  EXPECT_EQ(pb2.DebugString(), pb.DebugString());
}

