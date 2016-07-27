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
#include "retrieval/fuzzy/wavelet/wavelet_tree.h"


using std::string;
using namespace gdt::wavelet;

TEST(WaveletTreeTest, LookUp) {
  uint64_t temp[10] = {0,1,2,3,4,5,6,3,3,9};
  std::vector<uint64_t> array(temp, temp+10);
  WaveletTree wavelet_tree;
  wavelet_tree.Init(array);
}
