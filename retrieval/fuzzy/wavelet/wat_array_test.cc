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
#include "data_collector/feeder/candidate/similarity/wat_array.h"

using std::string;
using namespace wat_array;

TEST(WatArrayTest, LookUp) {
  uint64_t temp[10] = {0,1,2,3,4,5,6,3,3,9};
  std::vector<uint64_t> array(temp, temp+10);
  WatArray wat_array;
  wat_array.Init(array);
  EXPECT_EQ(wat_array.Lookup(4), 4);
  EXPECT_EQ(wat_array.Lookup(6), 6);
  EXPECT_EQ(wat_array.Rank(6, 0), 0);
  EXPECT_EQ(wat_array.Rank(3, 1), 0);
  EXPECT_EQ(wat_array.Select(4, 1), uint64_t(-1));
  EXPECT_EQ(wat_array.Select(4, 0), 0);
  EXPECT_EQ(wat_array.RankLessThan(4, 3), 3);
  EXPECT_EQ(wat_array.RankLessThan(4, 1), 1);
  EXPECT_EQ(wat_array.RankMoreThan(3, 2), 0);
  EXPECT_EQ(wat_array.RankMoreThan(8, 1), 0);
  uint64_t i = 0, j = 0, k = 0;
  wat_array.RankAll(1, 3, i , j , k);
  EXPECT_EQ(wat_array.FreqRange(8, 1, 1, 3), 0);
  wat_array.MaxRange(8, 1, j, k);
  wat_array.QuantileRange(8, 1, i, j, k);
  EXPECT_EQ(wat_array.alphabet_num(), 10);
  EXPECT_EQ(wat_array.Freq(3), 3);
  EXPECT_EQ(wat_array.Freq(1), 1);
  EXPECT_EQ(wat_array.length(), 10);
  std::ostream os(std::cout.rdbuf());
  std::istream is(std::cout.rdbuf());
  wat_array.Save(os);
  wat_array.Load(is);
}
