// Copyright 2015 Tencent Inc.
// Author: Guangneng Hu (lesliehu@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/word_embedding_dict.h"

#include "thirdparty/gtest/gtest.h"

namespace qzap {
namespace text_analysis {

static const float kEps = 1E-6;

TEST(WordEmbeddingDictTest, Build) {
  std::string invalid_embedding_file
      = "./testdata/embedding/invalid_qq_group-vec.50d.embedding";
  WordEmbeddingDict word_embedding_dict1;
  EXPECT_FALSE(word_embedding_dict1.Build(invalid_embedding_file));

  std::string embedding_file
      = "./testdata/embedding/qq_group-vec.50d.embedding";
  WordEmbeddingDict word_embedding_dict2;
  EXPECT_TRUE(word_embedding_dict2.Build(embedding_file));

  const EmbeddingInfo* embedding_info = word_embedding_dict2.Search("朋友");
  EXPECT_TRUE(embedding_info != NULL);
  EXPECT_EQ(50, embedding_info->embedding_size());
  EXPECT_NEAR(embedding_info->embedding(0), 0.349071, kEps);
  EXPECT_TRUE(word_embedding_dict2.Search("腾讯") == NULL);
}

TEST(WordEmbeddingDict, SaveAndLoad) {
  std::string embedding_file
      = "./testdata/embedding/qq_group-vec.50d.embedding";
  WordEmbeddingDict word_embedding_dict1;
  EXPECT_TRUE(word_embedding_dict1.Build(embedding_file));

  EXPECT_TRUE(word_embedding_dict1.Save("./testdata/dict.word_embedding"));

  WordEmbeddingDict word_embedding_dict2;
  EXPECT_TRUE(word_embedding_dict2.Load("./testdata/dict.word_embedding"));

  const EmbeddingInfo* embedding_info = word_embedding_dict2.Search("朋友");
  EXPECT_TRUE(embedding_info != NULL);
  EXPECT_EQ(50, embedding_info->embedding_size());
  EXPECT_NEAR(embedding_info->embedding(0), 0.349071, kEps);
  EXPECT_TRUE(word_embedding_dict2.Search("腾讯") == NULL);
}

}  // namespace text_analysis
}  // namespace qzap
