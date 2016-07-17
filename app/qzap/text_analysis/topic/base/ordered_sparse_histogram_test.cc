// Copyright (c) 2013 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/base/ordered_sparse_histogram.h"

#include <sstream>

#include "thirdparty/gtest/gtest.h"

#include "app/qzap/text_analysis/topic/base/cgo_types.h"
#include "app/qzap/text_analysis/topic/base/dense_topic_histogram.h"

namespace qzap {
namespace text_analysis {
namespace base {

static const int32_t kNumTopics = 6;
static const double kEpsilon = 1E-6;

TEST(OrderedSparseHistogramTest, Ctor) {
  // usage 1: the sparse_histogram should be allocated at construction time
  OrderedSparseHistogram sparse_histogram1(0);
  EXPECT_EQ(0, sparse_histogram1.NumTopics());
  EXPECT_EQ(0, sparse_histogram1.Length());

  // usage 2: the hist is already there in Go/C format
  ::OrderedSparseHistogram* hist1 = ::NewOrderedSparseHistogram(0);
  hist1->topics.array = new int32_t[3];
  hist1->topics.len = 0;
  hist1->topics.cap = 3;
  hist1->counts.array = new int64_t[3];
  hist1->counts.len = 0;
  hist1->counts.cap = 3;

  // the sparse_histogram will copy hist at construction time
  OrderedSparseHistogram sparse_histogram2(hist1, 3);
  EXPECT_EQ(3, sparse_histogram2.NumTopics());
  EXPECT_EQ(0, sparse_histogram1.Length());

  // Note: you must free histogram outside
  ::DeleteOrderedSparseHistogram(hist1);
  ::OrderedSparseHistogram* hist2 = sparse_histogram2.CType();
  ::DeleteOrderedSparseHistogram(hist2);
}

TEST(OrderedSparseHistogramTest, Serialize) {
  SparseTopicHistogramPB pb;
  SparseTopicHistogramPB::NonZero* nonzero = pb.add_nonzero();
  nonzero->set_topic(0);
  nonzero->set_count(10);
  nonzero = pb.add_nonzero();
  nonzero->set_topic(5);
  nonzero->set_count(5);

  ::OrderedSparseHistogram* hist = ::NewOrderedSparseHistogram(0);
  OrderedSparseHistogram sparse_histogram(hist, 0);

  EXPECT_EQ(0, sparse_histogram.NumTopics());
  sparse_histogram.ParseFrom(pb, kNumTopics);
  EXPECT_EQ(kNumTopics, sparse_histogram.NumTopics());
  ::DeleteOrderedSparseHistogram(hist);

  SparseTopicHistogramPB pb2;
  sparse_histogram.SerializeTo(&pb2);

  EXPECT_EQ(pb.DebugString(), pb2.DebugString());

  DenseTopicHistogram dense_histogram;
  dense_histogram.Resize(kNumTopics + 1);
  dense_histogram[0] = 5;
  dense_histogram[5] = 7;
  dense_histogram[4] = 1;
  sparse_histogram.ParseFrom(dense_histogram);
  ASSERT_EQ(dense_histogram.NumTopics(), sparse_histogram.NumTopics());
  for (int32_t i = 0; i < dense_histogram.NumTopics(); ++i) {
    EXPECT_EQ(dense_histogram[i], sparse_histogram.Count(i));
  }

  sparse_histogram.SerializeTo(&pb2);
}

TEST(OrderedSparseHistogramTest, IncrementDecrementTopic) {
  OrderedSparseHistogram sparse_histogram(kNumTopics);

  EXPECT_EQ(6, sparse_histogram.NumTopics());
  EXPECT_NEAR(0.0, sparse_histogram.SparseRatio(), kEpsilon);

  sparse_histogram.IncrementTopic(5, 5);
  EXPECT_TRUE(sparse_histogram.IsOrdered());
  sparse_histogram.IncrementTopic(0, 10);
  EXPECT_TRUE(sparse_histogram.IsOrdered());
  sparse_histogram.IncrementTopic(1, 1);
  EXPECT_TRUE(sparse_histogram.IsOrdered());

  EXPECT_EQ(5, sparse_histogram.Count(5));
  EXPECT_EQ(10, sparse_histogram.Count(0));
  EXPECT_EQ(1, sparse_histogram.Count(1));

  EXPECT_EQ(3, sparse_histogram.Length());
  EXPECT_EQ(6, sparse_histogram.NumTopics());
  EXPECT_NEAR(0.5, sparse_histogram.SparseRatio(), kEpsilon);

  sparse_histogram.DecrementTopic(0, 7);
  EXPECT_TRUE(sparse_histogram.IsOrdered());

  EXPECT_EQ(5, sparse_histogram.Count(5));
  EXPECT_EQ(3, sparse_histogram.Count(0));
  EXPECT_EQ(1, sparse_histogram.Count(1));

  EXPECT_EQ(3, sparse_histogram.Length());
  EXPECT_EQ(6, sparse_histogram.NumTopics());
  EXPECT_NEAR(0.5, sparse_histogram.SparseRatio(), kEpsilon);

  sparse_histogram.DecrementTopic(0, 3);
  EXPECT_TRUE(sparse_histogram.IsOrdered());

  EXPECT_EQ(5, sparse_histogram.Count(5));
  EXPECT_EQ(1, sparse_histogram.Count(1));

  EXPECT_EQ(2, sparse_histogram.Length());
  EXPECT_EQ(6, sparse_histogram.NumTopics());
  EXPECT_NEAR(0.333333333, sparse_histogram.SparseRatio(), kEpsilon);
}

TEST(OrderedSparseHistogramTest, Iterator) {
  OrderedSparseHistogram sparse_histogram(kNumTopics);

  sparse_histogram.IncrementTopic(5, 5);
  sparse_histogram.IncrementTopic(0, 10);
  sparse_histogram.IncrementTopic(1, 1);

  std::ostringstream o;
  for (OrderedSparseHistogram::ConstIterator it(sparse_histogram);
      !it.Done(); it.Next()) {
    o << "(" << it.Topic() << ',' << it.Count() << ") ";
  }

  const std::string sparse_histogram_str = "(0,10) (5,5) (1,1) ";
  EXPECT_EQ(o.str(), sparse_histogram_str);
}

TEST(OrderedSparseHistogramTest, IncreaseCapacity) {
  static const uint32_t kTotalTopics = 1500;
  OrderedSparseHistogram sparse_histogram(kTotalTopics);
  ::OrderedSparseHistogram* hist = sparse_histogram.CType();
  EXPECT_EQ(0u, sparse_histogram.Capacity());

  sparse_histogram.IncreaseCapacity();
  EXPECT_EQ(1u, sparse_histogram.Capacity());

  for (uint32_t i = 0; i < 1000; ++i) {
    EXPECT_EQ(hist->counts.len, hist->topics.len);
    EXPECT_EQ(hist->counts.cap, hist->topics.cap);
    if (hist->topics.len == sparse_histogram.Capacity()) {
      sparse_histogram.IncreaseCapacity();

      if (2 * hist->counts.len < kTotalTopics) {
        EXPECT_EQ(2 * hist->topics.len, sparse_histogram.Capacity());
        EXPECT_EQ(2 * hist->counts.len, sparse_histogram.Capacity());
      } else {
        EXPECT_EQ(kTotalTopics, sparse_histogram.Capacity());
        EXPECT_EQ(kTotalTopics, sparse_histogram.Capacity());
      }
    }
    ASSERT_LT(i, sparse_histogram.Capacity());
    static_cast<int32_t*>(hist->topics.array)[i] = i;
    static_cast<int64_t*>(hist->counts.array)[i] = 1;
    ++hist->topics.len;
    ++hist->counts.len;

    EXPECT_EQ(i + 1, hist->topics.len);
    EXPECT_EQ(i + 1, hist->counts.len);
    ASSERT_LE(hist->topics.len, sparse_histogram.Capacity());
    ASSERT_LE(hist->counts.len, sparse_histogram.Capacity());
  }
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
