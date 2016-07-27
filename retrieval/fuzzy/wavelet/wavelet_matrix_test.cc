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
#include "retrieval/fuzzy/wavelet/wavelet_matrix.h"

using std::string;
using namespace wat_array;
using namespace wavelet_matrix;

TEST(WaveletMatrixTest, LookUp) {
  uint64_t temp[10] = {0,1,2,3,4,5,6,3,3,9};
  std::vector<uint64_t> array(temp, temp+10);
  WaveletMatrix wavelet_matrix;
  wavelet_matrix.Init(array);
  EXPECT_EQ(wavelet_matrix.Lookup(4), 4);
  EXPECT_EQ(wavelet_matrix.Lookup(6), 6);
  EXPECT_EQ(wavelet_matrix.Rank(6, 0), 0);
  EXPECT_EQ(wavelet_matrix.Rank(3, 1), 0);
  EXPECT_EQ(wavelet_matrix.Select(4, 1), 5);
  EXPECT_EQ(wavelet_matrix.Select(4, 0), 1);
  EXPECT_EQ(wavelet_matrix.RankLessThan(4, 3), 3);
  EXPECT_EQ(wavelet_matrix.RankLessThan(4, 1), 1);
  EXPECT_EQ(wavelet_matrix.RankMoreThan(3, 2), 0);
  EXPECT_EQ(wavelet_matrix.RankMoreThan(8, 1), 0);
  uint64_t i = 0, j = 0, k = 0;
  wavelet_matrix.RankAll(1, 3, 3, i , j , k);
  EXPECT_EQ(wavelet_matrix.FreqRange(8, 1, 1, 3), 0);
  wavelet_matrix.MaxRange(8, 1, j, k);
  wavelet_matrix.QuantileRange(8, 1, i, j, k);
  EXPECT_EQ(wavelet_matrix.alphabet_num(), 10);
  EXPECT_EQ(wavelet_matrix.Freq(3), 3);
  EXPECT_EQ(wavelet_matrix.Freq(1), 1);
  EXPECT_EQ(wavelet_matrix.length(), 10);
  std::ostream os(std::cout.rdbuf());
  std::istream is(std::cout.rdbuf());
  wavelet_matrix.Save(os);
  wavelet_matrix.Load(is);
}
