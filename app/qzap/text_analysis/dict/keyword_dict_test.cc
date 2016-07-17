// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// unittest of KeywordDict class

#include "app/qzap/text_analysis/dict/keyword_dict.h"

#include "thirdparty/gtest/gtest.h"

namespace qzap {
namespace text_analysis {

class KeywordDictTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    CHECK(keyword_dict_.Build("testdata/keyword.dat"));
  }

  virtual void TearDown() {}

  KeywordDict keyword_dict_;
};

TEST_F(KeywordDictTest, Build) {
  const KeywordInfo* value = keyword_dict_.Search("什么射击网络游戏好玩");

  EXPECT_TRUE(value != NULL);
  EXPECT_FLOAT_EQ(0.123, value->weight());

  value = keyword_dict_.Search("");
  EXPECT_TRUE(value == NULL);

  value = keyword_dict_.Search("Nokia手机");
  EXPECT_TRUE(value != NULL);
  EXPECT_FLOAT_EQ(0.0, value->weight());
}

TEST_F(KeywordDictTest, ExtractByByte) {
  std::string test_text = "请问贺州哪儿牙齿美容的价格最便宜";
  std::vector<KeywordDict::ExtractResultType> result_vec;
  ASSERT_TRUE(keyword_dict_.ExtractByByte(test_text, &result_vec));
  EXPECT_EQ(3U, result_vec.size());

  EXPECT_EQ("贺州", test_text.substr(result_vec[0].boundaries[0],
          result_vec[0].boundaries.back() - result_vec[0].boundaries[0]));
  EXPECT_EQ("牙齿美容", test_text.substr(result_vec[1].boundaries[0],
      result_vec[1].boundaries.back() - result_vec[1].boundaries[0]));
  EXPECT_EQ("牙齿美容的价格", test_text.substr(result_vec[2].boundaries[0],
      result_vec[2].boundaries.back() - result_vec[2].boundaries[0]));

  result_vec.clear();
  ASSERT_TRUE(keyword_dict_.ExtractByByte("", &result_vec));
  EXPECT_EQ(0U, result_vec.size());
}

TEST_F(KeywordDictTest, ExtractByToken) {
  std::string test_text = "请问贺州哪儿牙齿美容的价格最便宜";
  std::vector<size_t> token_boundaries;
  token_boundaries.push_back(0);
  token_boundaries.push_back(6);
  token_boundaries.push_back(12);
  token_boundaries.push_back(18);
  token_boundaries.push_back(24);
  token_boundaries.push_back(30);
  token_boundaries.push_back(33);
  token_boundaries.push_back(39);
  token_boundaries.push_back(45);
  token_boundaries.push_back(48);
  token_boundaries.push_back(54);
  std::vector<KeywordDict::ExtractResultType> results;

  ASSERT_TRUE(keyword_dict_.ExtractByToken(test_text,
                                           token_boundaries,
                                           &results));
  EXPECT_EQ(3U, results.size());

  const std::vector<size_t>& b0 = results[0].boundaries;
  const std::vector<size_t>& b1 = results[1].boundaries;
  const std::vector<size_t>& b2 = results[2].boundaries;
  EXPECT_EQ("贺州", test_text.substr(b0[0], b0.back() - b0[0]));
  EXPECT_EQ("牙齿美容", test_text.substr(b1[0], b1.back() - b1[0]));
  EXPECT_EQ("牙齿美容的价格", test_text.substr(b2[0], b2.back() - b2[0]));

  EXPECT_EQ(2U, b0.size());
  EXPECT_EQ("贺州", test_text.substr(b0[0], b0[1] - b0[0]));

  EXPECT_EQ(3U, b1.size());
  EXPECT_EQ("牙齿", test_text.substr(b1[0], b1[1] - b1[0]));
  EXPECT_EQ("美容", test_text.substr(b1[1], b1[2] - b1[1]));

  EXPECT_EQ(5U, b2.size());
  EXPECT_EQ("牙齿", test_text.substr(b2[0], b2[1] - b2[0]));
  EXPECT_EQ("美容", test_text.substr(b2[1], b2[2] - b2[1]));
  EXPECT_EQ("的", test_text.substr(b2[2], b2[3] - b2[2]));
  EXPECT_EQ("价格", test_text.substr(b2[3], b2[4] - b2[3]));
}

TEST_F(KeywordDictTest, SaveAndLoad) {
  keyword_dict_.Save("testdata/dict.keyword");
  keyword_dict_.Clear();
  keyword_dict_.Load("testdata/dict.keyword");

  const KeywordInfo* value = keyword_dict_.Search("什么射击网络游戏好玩");

  EXPECT_TRUE(value != NULL);
  EXPECT_FLOAT_EQ(0.123, value->weight());

  value = keyword_dict_.Search("");
  EXPECT_TRUE(value == NULL);

  value = keyword_dict_.Search("Nokia手机");
  EXPECT_TRUE(value != NULL);
  EXPECT_FLOAT_EQ(0.0, value->weight());
}

}  // namespace text_analysis
}  // namespace qzap

