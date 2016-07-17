// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// the unittest of StopwordDict class

#include "app/qzap/text_analysis/dict/stopword_dict.h"

#include <string>

#include "thirdparty/gtest/gtest.h"

namespace qzap {
namespace text_analysis {

TEST(StopwordDict, IsStopword) {
  StopwordDict stopword_dict;
  ASSERT_TRUE(stopword_dict.Load("testdata/dict.stopword"));

  EXPECT_TRUE(stopword_dict.IsStopword("可以"));
  EXPECT_TRUE(stopword_dict.IsStopword("呢"));
  EXPECT_TRUE(stopword_dict.IsStopword("的"));
  EXPECT_TRUE(stopword_dict.IsStopword("多少"));
  EXPECT_FALSE(stopword_dict.IsStopword("hello world"));
  EXPECT_TRUE(stopword_dict.IsStopword("a"));
  EXPECT_TRUE(stopword_dict.IsStopword("。"));
  EXPECT_FALSE(stopword_dict.IsStopword(""));
  EXPECT_FALSE(stopword_dict.IsStopword("小游戏"));
  EXPECT_FALSE(stopword_dict.IsStopword("腾讯科技"));
  EXPECT_FALSE(stopword_dict.IsStopword("苹果iphone"));
  EXPECT_FALSE(stopword_dict.IsStopword("iphone 4s"));
  EXPECT_FALSE(stopword_dict.IsStopword("凡客"));
}

}  // namespace text_analysis
}  // namespace qzap
