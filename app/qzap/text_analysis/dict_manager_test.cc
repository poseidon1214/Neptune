// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict_manager.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/segmenter/segmenter.h"

DECLARE_string(segmenter_data_dir);

namespace qzap {
namespace text_analysis {

static const float kEps = 1E-6;

TEST(DictManager, Init) {
  DictManager dict_manager;
  EXPECT_TRUE(dict_manager.Init("testdata/kedict"));
}

class DictManagerTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    CHECK(dict_manager_.Init("testdata/kedict"));
  }
  virtual void TearDown() {}

  DictManager dict_manager_;
};

TEST_F(DictManagerTest, KeywordDict) {
  const KeywordDict* keyword_dict_ptr = dict_manager_.GetKeywordDict();
  ASSERT_TRUE(keyword_dict_ptr != NULL);

  const KeywordInfo* value = keyword_dict_ptr->Search("");
  EXPECT_TRUE(value == NULL);
  value = keyword_dict_ptr->Search("Nokia手机");
  EXPECT_TRUE(value != NULL);
}

TEST_F(DictManagerTest, KeywordBlacklist) {
  const Vocabulary* keyword_blacklist_ptr = dict_manager_.GetKeywordBlacklist();
  ASSERT_TRUE(keyword_blacklist_ptr != NULL);

  EXPECT_TRUE(keyword_blacklist_ptr->HasWord("广州"));
  EXPECT_TRUE(keyword_blacklist_ptr->HasWord("网站"));
  EXPECT_TRUE(keyword_blacklist_ptr->HasWord("图片"));
  EXPECT_TRUE(keyword_blacklist_ptr->HasWord("统一"));
  EXPECT_FALSE(keyword_blacklist_ptr->HasWord("鲜花快递"));
}

TEST_F(DictManagerTest, StopwordDict) {
  const StopwordDict* stopword_dict_ptr = dict_manager_.GetStopwordDict();
  ASSERT_TRUE(stopword_dict_ptr != NULL);

  EXPECT_TRUE(stopword_dict_ptr->IsStopword("可以"));
  EXPECT_FALSE(stopword_dict_ptr->IsStopword("凡客"));
}

TEST_F(DictManagerTest, TokenIdfDict) {
  const TokenIdfDict* token_idf_dict_ptr = dict_manager_.GetTokenIdfDict();
  ASSERT_TRUE(token_idf_dict_ptr != NULL);

  const IdfInfo* idf_info = token_idf_dict_ptr->Search("酒店");
  EXPECT_TRUE(idf_info != NULL);

  EXPECT_TRUE(token_idf_dict_ptr->Search("那么") == NULL);
}

TEST_F(DictManagerTest, WordEmbeddingDict) {
  const WordEmbeddingDict* word_embedding_dict_ptr
      = dict_manager_.GetWordEmbeddingDict();
  ASSERT_TRUE(word_embedding_dict_ptr != NULL);

  const EmbeddingInfo* embedding_info = word_embedding_dict_ptr->Search("朋友");
  EXPECT_TRUE(embedding_info != NULL);
  EXPECT_EQ(50, embedding_info->embedding_size());
  EXPECT_NEAR(embedding_info->embedding(0), 0.349071, kEps);
  EXPECT_TRUE(word_embedding_dict_ptr->Search("腾讯") == NULL);
}

}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);
  ::testing::InitGoogleTest(&argc, argv);

  FLAGS_segmenter_data_dir = "testdata/tc_data";

  return RUN_ALL_TESTS();
}

