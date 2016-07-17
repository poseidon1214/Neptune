// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
// Author: Wang Qian <cernwang@tencent.com>

#include <string>
#include <map>
#include <utility>

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"
#include "creative/dynamic_creative/candidate/similarity/similarity_retrievaler.h"
#include "creative/dynamic_creative/candidate/similarity/commodity_similarity.h"

using std::string;
using namespace gdt::dynamic_creative;

TEST(CommoditySimilarity, SimilarityRetrievalerTest) {
  SimilarityRetrievaler similarity_retrievaler;
  similarity_retrievaler.BuildWithLabels("feature");
  similarity_retrievaler.RetrievalWithInputFile("whole_feature", "invert_similar");
  similarity_retrievaler.DocmentsToTopKListWithLabels("whole_feature", "invert_similar", 0.1);
}

TEST(CommoditySimilarity, CommoditySimilarityTest) {
  CommoditySimilarity commodity_similarity;
  std::vector<Commodity> commodities;
  commodity_similarity.WriteDocmentFeatures("empty_file_2", commodities);
  commodity_similarity.WriteDocmentFeaturesWithLocalId("empty_file_2", commodities);
  commodity_similarity.InvertFileToForwardFile("empty_file_2", "empty_file_2");
  commodity_similarity.ReadSimilarLists("empty_file_2", &commodities);
  commodity_similarity.ReadSimilarLocalIdLists("empty_file_2", &commodities);
  commodity_similarity.ComputeSimilarities("empty_file_2", "empty_file_2", "empty_file_2");
}
