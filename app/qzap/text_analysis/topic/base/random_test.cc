// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Yi Wang (yiwang@tencent.com)

#include "app/qzap/text_analysis/topic/base/random.h"

#include <numeric>
#include <vector>

#include "thirdparty/gtest/gtest.h"

using qzap::text_analysis::base::MTRandom;

double Variance(const std::vector<int32_t>& array) {
  double mean = std::accumulate(array.begin(), array.end(), 0) /
      static_cast<double>(array.size());

  double variance = 0.0;
  for (size_t i = 0; i < array.size(); ++i) {
    variance += (array[i] - mean) * (array[i] - mean);
  }
  return variance / array.size();
}

TEST(MTRandomTest, RandInt32) {
  const int32_t kCount = 100000;
  const int32_t kBound = 100;

  MTRandom rng;
  rng.SeedRNG(0);

  std::vector<int32_t> hist(kBound);
  for (int32_t i = 0; i < kCount; ++i) {
    int32_t value = rng.RandInt32(kBound);
    ++hist[value];
  }

  EXPECT_LT(Variance(hist), 1000.0);
}
