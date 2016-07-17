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
#include "data_collector/feeder/candidate/similarity/wavelet_tree.h"


using std::string;
using namespace gdt::dynamic_creative;

TEST(WaveletTreeTest, LookUp) {
  uint64_t temp[10] = {0,1,2,3,4,5,6,3,3,9};
  std::vector<uint64_t> array(temp, temp+10);
  WaveletTree wavelet_tree;
  wavelet_tree.Init(array);
  std::vector<std::pair<size_t, size_t> > patterns;
  patterns.push_back(std::make_pair(0,9));
  std::vector<int64_t> results;
  wavelet_tree.Intersect(patterns,1,1, &results);
  EXPECT_EQ(results.size(), 1);
  std::vector<std::pair<std::pair<size_t, size_t>, double> > origin_patterns;
  origin_patterns.push_back(std::make_pair(std::make_pair(2,5), 0.3));
  wavelet_tree.RetrievalWithFilter(origin_patterns,patterns,0.3,&results);
  EXPECT_EQ(results.size(), 4);
  wavelet_tree.Retrieval(origin_patterns,0.3,&results);
  EXPECT_EQ(results.size(), 7);
}
