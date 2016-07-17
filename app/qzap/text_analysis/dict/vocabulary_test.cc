// Copyright 2011, Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/vocabulary.h"

#include <string>

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

DECLARE_bool(using_default_index);

namespace qzap {
namespace text_analysis {

TEST(Vocabulary, BuildUsingDefaultIndex) {
  std::string voc_file = "testdata/vocabulary/word.txt";
  Vocabulary vocabulary;
  EXPECT_TRUE(vocabulary.Build(voc_file));
  EXPECT_EQ(5, vocabulary.Size());

  EXPECT_TRUE(vocabulary.HasWord("鲜花"));
  EXPECT_TRUE(vocabulary.HasWord("鲜花速递公司"));
  EXPECT_TRUE(vocabulary.HasWord("腾讯科技"));
  EXPECT_TRUE(vocabulary.HasWord("iphone"));
  EXPECT_TRUE(vocabulary.HasWord("iphone 4s"));
  EXPECT_FALSE(vocabulary.HasWord("腾讯"));
  EXPECT_FALSE(vocabulary.HasWord("tencent"));
  EXPECT_FALSE(vocabulary.HasWord(""));

  EXPECT_EQ(0, vocabulary.WordIndex("鲜花"));
  EXPECT_EQ(1, vocabulary.WordIndex("鲜花速递公司"));
  EXPECT_EQ(2, vocabulary.WordIndex("腾讯科技"));
  EXPECT_EQ(3, vocabulary.WordIndex("iphone"));
  EXPECT_EQ(4, vocabulary.WordIndex("iphone 4s"));
  EXPECT_EQ(-1, vocabulary.WordIndex("腾讯"));
  EXPECT_EQ(-1, vocabulary.WordIndex("tencent"));
  EXPECT_EQ(-1, vocabulary.WordIndex(""));

  EXPECT_EQ("鲜花", vocabulary.Word(0));
  EXPECT_EQ("鲜花速递公司", vocabulary.Word(1));
  EXPECT_EQ("腾讯科技", vocabulary.Word(2));
  EXPECT_EQ("iphone", vocabulary.Word(3));
  EXPECT_EQ("iphone 4s", vocabulary.Word(4));
}

TEST(Vocabulary, BuildNotUsingDefaultIndex) {
  FLAGS_using_default_index = false;  // 文件中每行包含word 及其 index

  std::string voc_file = "testdata/vocabulary/word.txt";
  Vocabulary vocabulary;
  EXPECT_TRUE(vocabulary.Build(voc_file));
  EXPECT_EQ(11, vocabulary.Size());

  EXPECT_TRUE(vocabulary.HasWord("鲜花"));
  EXPECT_TRUE(vocabulary.HasWord("鲜花速递公司"));
  EXPECT_FALSE(vocabulary.HasWord("腾讯科技"));
  EXPECT_FALSE(vocabulary.HasWord("iphone"));
  EXPECT_TRUE(vocabulary.HasWord("iphone 4s"));
  EXPECT_FALSE(vocabulary.HasWord("腾讯"));
  EXPECT_FALSE(vocabulary.HasWord("tencent"));
  EXPECT_FALSE(vocabulary.HasWord(""));

  EXPECT_EQ(3, vocabulary.WordIndex("鲜花"));
  EXPECT_EQ(10, vocabulary.WordIndex("鲜花速递公司"));
  EXPECT_EQ(-1, vocabulary.WordIndex("腾讯科技"));
  EXPECT_EQ(-1, vocabulary.WordIndex("iphone"));
  EXPECT_EQ(5, vocabulary.WordIndex("iphone 4s"));
  EXPECT_EQ(-1, vocabulary.WordIndex("腾讯"));
  EXPECT_EQ(-1, vocabulary.WordIndex("tencent"));
  EXPECT_EQ(-1, vocabulary.WordIndex(""));

  EXPECT_EQ("", vocabulary.Word(1));
  EXPECT_EQ("鲜花", vocabulary.Word(3));
  EXPECT_EQ("鲜花速递公司", vocabulary.Word(10));
  EXPECT_EQ("iphone 4s", vocabulary.Word(5));

  FLAGS_using_default_index = true;
}

TEST(Vocabulary, SaveAndLoad) {
  std::string voc_file = "testdata/vocabulary/word.txt";
  Vocabulary vocabulary;
  EXPECT_TRUE(vocabulary.Build(voc_file));
  EXPECT_TRUE(vocabulary.Save("dict.vocabulary"));
  EXPECT_TRUE(vocabulary.Load("dict.vocabulary"));
  EXPECT_EQ(5, vocabulary.Size());

  EXPECT_TRUE(vocabulary.HasWord("鲜花"));
  EXPECT_TRUE(vocabulary.HasWord("鲜花速递公司"));
  EXPECT_TRUE(vocabulary.HasWord("腾讯科技"));
  EXPECT_TRUE(vocabulary.HasWord("iphone"));
  EXPECT_TRUE(vocabulary.HasWord("iphone 4s"));
  EXPECT_FALSE(vocabulary.HasWord("腾讯"));
  EXPECT_FALSE(vocabulary.HasWord("tencent"));
  EXPECT_FALSE(vocabulary.HasWord(""));

  EXPECT_EQ(0, vocabulary.WordIndex("鲜花"));
  EXPECT_EQ(1, vocabulary.WordIndex("鲜花速递公司"));
  EXPECT_EQ(2, vocabulary.WordIndex("腾讯科技"));
  EXPECT_EQ(3, vocabulary.WordIndex("iphone"));
  EXPECT_EQ(4, vocabulary.WordIndex("iphone 4s"));
  EXPECT_EQ(-1, vocabulary.WordIndex("腾讯"));
  EXPECT_EQ(-1, vocabulary.WordIndex("tencent"));
  EXPECT_EQ(-1, vocabulary.WordIndex(""));

  EXPECT_EQ("鲜花", vocabulary.Word(0));
  EXPECT_EQ("鲜花速递公司", vocabulary.Word(1));
  EXPECT_EQ("腾讯科技", vocabulary.Word(2));
  EXPECT_EQ("iphone", vocabulary.Word(3));
  EXPECT_EQ("iphone 4s", vocabulary.Word(4));
}

TEST(Vocabulary, ExtractByByte) {
  std::string voc_file = "testdata/vocabulary/word.txt";
  Vocabulary vocabulary;
  EXPECT_TRUE(vocabulary.Build(voc_file));
  std::string test_text = "腾讯科技有限公司，iphone 4s发布，iphones牌鲜花速递";

  std::vector<Vocabulary::ExtractResultType> result_vec;
  EXPECT_TRUE(vocabulary.ExtractByByte(test_text, &result_vec));

  EXPECT_EQ(4U, result_vec.size());
  EXPECT_EQ("腾讯科技", test_text.substr(
          result_vec[0].boundaries[0],
          result_vec[0].boundaries.back() - result_vec[0].boundaries[0]));
  EXPECT_EQ("iphone", test_text.substr(
          result_vec[1].boundaries[0],
          result_vec[1].boundaries.back() - result_vec[1].boundaries[0]));
  EXPECT_EQ("iphone 4s", test_text.substr(
          result_vec[2].boundaries[0],
          result_vec[2].boundaries.back() - result_vec[2].boundaries[0]));
  EXPECT_EQ("鲜花", test_text.substr(
          result_vec[3].boundaries[0],
          result_vec[3].boundaries.back() - result_vec[3].boundaries[0]));

  result_vec.clear();
  EXPECT_TRUE(vocabulary.ExtractByByte("", &result_vec));
  EXPECT_EQ(0U, result_vec.size());
}

TEST(Vocabulary, ExtractByToken) {
  std::string voc_file = "testdata/vocabulary/word.txt";
  Vocabulary vocabulary;
  EXPECT_TRUE(vocabulary.Build(voc_file));

  std::string test_text = "腾讯科技有限公司，iphone 4s发布，iphones牌鲜花速递";

  // 模拟分词边界
  std::vector<size_t> token_boundaries;
  token_boundaries.push_back(0);
  token_boundaries.push_back(6);
  token_boundaries.push_back(12);
  token_boundaries.push_back(18);
  token_boundaries.push_back(24);
  token_boundaries.push_back(27);
  token_boundaries.push_back(36);
  token_boundaries.push_back(42);
  token_boundaries.push_back(45);
  token_boundaries.push_back(52);
  token_boundaries.push_back(55);
  token_boundaries.push_back(61);
  token_boundaries.push_back(67);
  std::vector<Vocabulary::ExtractResultType> result_vec;
  EXPECT_TRUE(vocabulary.ExtractByToken(test_text, token_boundaries,
                                        &result_vec));

  EXPECT_EQ(3U, result_vec.size());
  EXPECT_EQ("腾讯科技", test_text.substr(
          result_vec[0].boundaries[0],
          result_vec[0].boundaries.back() - result_vec[0].boundaries[0]));
  EXPECT_EQ("iphone 4s", test_text.substr(
          result_vec[1].boundaries[0],
          result_vec[1].boundaries.back() - result_vec[1].boundaries[0]));
  EXPECT_EQ("鲜花", test_text.substr(
          result_vec[2].boundaries[0],
          result_vec[2].boundaries.back() - result_vec[2].boundaries[0]));

  result_vec.clear();
  EXPECT_TRUE(vocabulary.ExtractByByte("", &result_vec));
  EXPECT_EQ(0U, result_vec.size());
}

}  // namespace text_analysis
}  // namespace qzap
