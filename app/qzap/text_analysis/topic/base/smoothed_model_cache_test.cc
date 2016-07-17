// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/base/smoothed_model_cache.h"

#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/topic/base/model.h"

namespace qzap {
namespace text_analysis {
namespace base {

const static char kModelDir[] = "testdata/model-standard";

TEST(SmoothedModelCacheTest, GetProbWordGivenTopic) {
  Model model(2);
  ASSERT_EQ(0, model.Load(kModelDir));

  SmoothedModelCache smoothed_model_cache;
  smoothed_model_cache.Compute(model, 0);
  EXPECT_TRUE(smoothed_model_cache.GetProbWordGivenTopic(0) == NULL);
  EXPECT_TRUE(smoothed_model_cache.GetProbWordGivenTopic(1) == NULL);
  EXPECT_TRUE(smoothed_model_cache.GetProbWordGivenTopic(2) == NULL);
  EXPECT_TRUE(smoothed_model_cache.GetProbWordGivenTopic(3) == NULL);
  EXPECT_TRUE(smoothed_model_cache.GetProbWordGivenTopic(4) == NULL);

  smoothed_model_cache.Compute(model, 5);
  EXPECT_TRUE(smoothed_model_cache.GetProbWordGivenTopic(0) != NULL);
  EXPECT_TRUE(smoothed_model_cache.GetProbWordGivenTopic(1) != NULL);
  EXPECT_TRUE(smoothed_model_cache.GetProbWordGivenTopic(2) != NULL);
  EXPECT_TRUE(smoothed_model_cache.GetProbWordGivenTopic(3) != NULL);
  EXPECT_TRUE(smoothed_model_cache.GetProbWordGivenTopic(4) != NULL);
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
