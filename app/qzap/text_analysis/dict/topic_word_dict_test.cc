// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// the unittest of TopicWordDict class

#include "app/qzap/text_analysis/dict/topic_word_dict.h"

#include <string>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace qzap {
namespace text_analysis {

TEST(TopicWordDict, Load) {
  TopicWordDict topic_word_dict;
  EXPECT_TRUE(topic_word_dict.Load("testdata/dict.topic_word"));
  EXPECT_FALSE(topic_word_dict.Load("testdata/invalid_dict.topic_word"));

  EXPECT_TRUE(topic_word_dict.LoadFromTextFile("testdata/topic_word.dat"));
  EXPECT_FALSE(topic_word_dict.LoadFromTextFile(
          "testdata/invalid_topic_word.dat"));
}

TEST(TopicWordDict, GetTopicWords) {
  TopicWordDict topic_word_dict;
  ASSERT_TRUE(topic_word_dict.Load("testdata/dict.topic_word"));

  std::string words;
  EXPECT_TRUE(topic_word_dict.GetTopicWords(69, &words));
  LOG(ERROR) << words;
  EXPECT_EQ("{诗:0.032293, 字:0.00943551, 藏头:0.00648807, 花:0.00584433, "
            "风:0.00575696, 春:0.0056558, 水:0.00533853, 雨:0.00522817, "
            "下联:0.00503965, 作者:0.00497987}", words);

  EXPECT_TRUE(topic_word_dict.GetTopicWords(443, &words));
  EXPECT_EQ("{诺基亚:0.0842359, 手机:0.0542206, 钱:0.0434808, 行货:0.0417942, "
            "水货:0.0315763, 价格:0.022649, 报价:0.0136053, 港行:0.00997566, "
            "地区:0.00860116, 便宜:0.0085965}", words);

  EXPECT_TRUE(topic_word_dict.GetTopicWords(761, &words));
  EXPECT_EQ("{车:0.0509835, 汽车:0.0236986, 钱:0.0211002, 发动机:0.014583, "
            "油:0.0104646, 价格:0.0078948, 手动:0.00654576, 东风:0.00634625, "
            "大众:0.00546748, 原因:0.00510171}", words);

  EXPECT_TRUE(topic_word_dict.GetTopKTopicWords(443, 10, &words));
  EXPECT_EQ("{诺基亚:0.0842359, 手机:0.0542206, 钱:0.0434808, 行货:0.0417942, "
            "水货:0.0315763, 价格:0.022649, 报价:0.0136053, 港行:0.00997566, "
            "地区:0.00860116, 便宜:0.0085965}", words);

  EXPECT_TRUE(topic_word_dict.GetTopKTopicWords(443, 6, &words));
  EXPECT_EQ("{诺基亚:0.0842359, 手机:0.0542206, 钱:0.0434808, 行货:0.0417942, "
            "水货:0.0315763, 价格:0.022649}", words);

  EXPECT_TRUE(topic_word_dict.GetTopKTopicWords(443, 30, &words));
  EXPECT_EQ("{诺基亚:0.0842359, 手机:0.0542206, 钱:0.0434808, 行货:0.0417942, "
            "水货:0.0315763, 价格:0.022649, 报价:0.0136053, 港行:0.00997566, "
            "地区:0.00860116, 便宜:0.0085965, 区别:0.00834955, "
            "nokia:0.00745962, n85:0.00711483, n78:0.00645321, n86:0.00631343, "
            "最新:0.00608046, 三星:0.00596864, 机:0.00595932, 店:0.00574033, "
            "n95:0.00559589, 功能:0.005526, n81:0.00550737, 大陆:0.00524179, "
            "n97:0.00489234, 价钱:0.00486438, 市场:0.00486438, 二手:0.00471528,"
            " 货:0.00466869, 网上:0.00449163, 价位:0.00445436}", words);
}

}  // namespace text_analysis
}  // namespace qzap

