// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/document_utils.h"

#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

namespace qzap {
namespace text_analysis {

static const double kEpsilon = 1E-6;

TEST(DocumentUtilsTest, WeightGreater) {
  Token lhs, rhs;
  lhs.set_weight(1.0);
  rhs.set_weight(1.0);
  EXPECT_FALSE(WeightGreater<Token>()(lhs, rhs));

  lhs.set_weight(2.0);
  rhs.set_weight(1.0);
  EXPECT_TRUE(WeightGreater<Token>()(lhs, rhs));

  lhs.set_weight(0.0);
  rhs.set_weight(1.0);
  EXPECT_FALSE(WeightGreater<Token>()(lhs, rhs));
}

TEST(DocumentUtilsTest, L1Normalize) {
  google::protobuf::RepeatedPtrField<Token> tokens;

  Token* token = tokens.Add();
  token->set_weight(1.0);
  token = tokens.Add();
  token->set_weight(2.0);
  token = tokens.Add();
  token->set_weight(3.0);
  token = tokens.Add();
  token->set_weight(4.0);

  DocumentUtils::L1Normalize(&tokens);

  EXPECT_NEAR(0.1, tokens.Get(0).weight(), kEpsilon);
  EXPECT_NEAR(0.2, tokens.Get(1).weight(), kEpsilon);
  EXPECT_NEAR(0.3, tokens.Get(2).weight(), kEpsilon);
  EXPECT_NEAR(0.4, tokens.Get(3).weight(), kEpsilon);
}

TEST(DocumentUtilsTest, L2Normalize) {
  google::protobuf::RepeatedPtrField<Token> tokens;

  Token* token = tokens.Add();
  token->set_weight(1.0);
  token = tokens.Add();
  token->set_weight(2.0);
  token = tokens.Add();
  token->set_weight(3.0);
  token = tokens.Add();
  token->set_weight(4.0);

  DocumentUtils::L2Normalize(&tokens);

  EXPECT_NEAR(0.18257418274879456, tokens.Get(0).weight(), kEpsilon);
  EXPECT_NEAR(0.36514836549758911, tokens.Get(1).weight(), kEpsilon);
  EXPECT_NEAR(0.54772257804870605, tokens.Get(2).weight(), kEpsilon);
  EXPECT_NEAR(0.73029673099517822, tokens.Get(3).weight(), kEpsilon);
}

TEST(DocumentUtilsTest, TruncateRepeatedFieldForTopK) {
  google::protobuf::RepeatedPtrField<Token> tokens;

  Token* token = tokens.Add();
  token->set_weight(0.1);
  token = tokens.Add();
  token->set_weight(0.2);
  token = tokens.Add();
  token->set_weight(0.3);
  token = tokens.Add();
  token->set_weight(0.4);
  token = tokens.Add();
  token->set_weight(1.0);
  token = tokens.Add();
  token->set_weight(2.0);
  token = tokens.Add();
  token->set_weight(3.0);
  token = tokens.Add();
  token->set_weight(4.0);

  DocumentUtils::TruncateRepeatedField(4, &tokens);

  ASSERT_EQ(4, tokens.size());
  EXPECT_NEAR(4.0, tokens.Get(0).weight(), kEpsilon);
  EXPECT_NEAR(3.0, tokens.Get(1).weight(), kEpsilon);
  EXPECT_NEAR(2.0, tokens.Get(2).weight(), kEpsilon);
  EXPECT_NEAR(1.0, tokens.Get(3).weight(), kEpsilon);

  DocumentUtils::TruncateRepeatedField(10, &tokens);
  ASSERT_EQ(4, tokens.size());
}

TEST(DocumentUtilsTest, TruncateRepeatedFieldForThreshold) {
  google::protobuf::RepeatedPtrField<Token> tokens;

  Token* token = tokens.Add();
  token->set_weight(0.1);
  token = tokens.Add();
  token->set_weight(0.2);
  token = tokens.Add();
  token->set_weight(0.3);
  token = tokens.Add();
  token->set_weight(0.4);
  token = tokens.Add();
  token->set_weight(1.0);
  token = tokens.Add();
  token->set_weight(2.0);
  token = tokens.Add();
  token->set_weight(3.0);
  token = tokens.Add();
  token->set_weight(4.0);

  DocumentUtils::TruncateRepeatedField(1.0, &tokens);

  ASSERT_EQ(4, tokens.size());
  EXPECT_NEAR(4.0, tokens.Get(0).weight(), kEpsilon);
  EXPECT_NEAR(3.0, tokens.Get(1).weight(), kEpsilon);
  EXPECT_NEAR(2.0, tokens.Get(2).weight(), kEpsilon);
  EXPECT_NEAR(1.0, tokens.Get(3).weight(), kEpsilon);

  DocumentUtils::TruncateRepeatedField(0.0, &tokens);
  ASSERT_EQ(4, tokens.size());
}

TEST(DocumentUtilsTest, TruncateAndL2NormRepeatedFieldForTopK) {
  google::protobuf::RepeatedPtrField<Token> tokens;

  Token* token = tokens.Add();
  token->set_weight(0.1);
  token = tokens.Add();
  token->set_weight(0.2);
  token = tokens.Add();
  token->set_weight(0.3);
  token = tokens.Add();
  token->set_weight(0.4);
  token = tokens.Add();
  token->set_weight(1.0);
  token = tokens.Add();
  token->set_weight(2.0);
  token = tokens.Add();
  token->set_weight(3.0);
  token = tokens.Add();
  token->set_weight(4.0);

  DocumentUtils::TruncateAndL2NormRepeatedField(4, &tokens);

  ASSERT_EQ(4, tokens.size());
  EXPECT_NEAR(0.73029673099517822, tokens.Get(0).weight(), kEpsilon);
  EXPECT_NEAR(0.54772257804870605, tokens.Get(1).weight(), kEpsilon);
  EXPECT_NEAR(0.36514836549758911, tokens.Get(2).weight(), kEpsilon);
  EXPECT_NEAR(0.18257418274879456, tokens.Get(3).weight(), kEpsilon);

  DocumentUtils::TruncateRepeatedField(10, &tokens);
  ASSERT_EQ(4, tokens.size());
}

TEST(DocumentUtilsTest, TruncateAndL2NormRepeatedFieldForThresholdAndTopK) {
  google::protobuf::RepeatedPtrField<Token> tokens;

  Token* token = tokens.Add();
  token->set_weight(0.1);
  token = tokens.Add();
  token->set_weight(0.2);
  token = tokens.Add();
  token->set_weight(0.3);
  token = tokens.Add();
  token->set_weight(0.4);
  token = tokens.Add();
  token->set_weight(1.0);
  token = tokens.Add();
  token->set_weight(2.0);
  token = tokens.Add();
  token->set_weight(3.0);
  token = tokens.Add();
  token->set_weight(4.0);

  DocumentUtils::TruncateAndL2NormRepeatedField(3, 1.0, &tokens);

  ASSERT_EQ(3, tokens.size());
  EXPECT_NEAR(0.742781, tokens.Get(0).weight(), kEpsilon);
  EXPECT_NEAR(0.557086, tokens.Get(1).weight(), kEpsilon);
  EXPECT_NEAR(0.371391, tokens.Get(2).weight(), kEpsilon);

  DocumentUtils::TruncateRepeatedField(10, &tokens);
  ASSERT_EQ(3, tokens.size());

  google::protobuf::RepeatedPtrField<Token> tokens2;
  Token* token2 = tokens2.Add();
  token2->set_weight(0.1);
  token2 = tokens2.Add();
  token2->set_weight(1.0);
  token2 = tokens2.Add();
  token2->set_weight(2.0);
  token2 = tokens2.Add();
  token2->set_weight(3.0);
  token2 = tokens2.Add();
  token2->set_weight(4.0);
  token2 = tokens2.Add();
  token2->set_weight(0.2);
  token2 = tokens2.Add();
  token2->set_weight(0.3);
  token2 = tokens2.Add();
  token2->set_weight(0.4);

  DocumentUtils::TruncateAndL2NormRepeatedField(4, 0.0, &tokens2);

  ASSERT_EQ(4, tokens2.size());
  EXPECT_NEAR(0.73029673099517822, tokens2.Get(0).weight(), kEpsilon);
  EXPECT_NEAR(0.54772257804870605, tokens2.Get(1).weight(), kEpsilon);
  EXPECT_NEAR(0.36514836549758911, tokens2.Get(2).weight(), kEpsilon);
  EXPECT_NEAR(0.18257418274879456, tokens2.Get(3).weight(), kEpsilon);

  DocumentUtils::TruncateRepeatedField(10, &tokens);
  ASSERT_EQ(4, tokens2.size());
}

TEST(DocumentUtilsTest, TruncateAndL2NormRepeatedFieldForThreshold) {
  google::protobuf::RepeatedPtrField<Token> tokens;

  Token* token = tokens.Add();
  token->set_weight(0.1);
  token = tokens.Add();
  token->set_weight(0.2);
  token = tokens.Add();
  token->set_weight(0.3);
  token = tokens.Add();
  token->set_weight(0.4);
  token = tokens.Add();
  token->set_weight(1.0);
  token = tokens.Add();
  token->set_weight(2.0);
  token = tokens.Add();
  token->set_weight(3.0);
  token = tokens.Add();
  token->set_weight(4.0);

  DocumentUtils::TruncateAndL2NormRepeatedField(1.0, &tokens);

  ASSERT_EQ(4, tokens.size());
  EXPECT_NEAR(0.73029673099517822, tokens.Get(0).weight(), kEpsilon);
  EXPECT_NEAR(0.54772257804870605, tokens.Get(1).weight(), kEpsilon);
  EXPECT_NEAR(0.36514836549758911, tokens.Get(2).weight(), kEpsilon);
  EXPECT_NEAR(0.18257418274879456, tokens.Get(3).weight(), kEpsilon);

  DocumentUtils::TruncateRepeatedField(10, &tokens);
  ASSERT_EQ(4, tokens.size());
}

TEST(DocumentUtilsTest, GetAndL2NormLevelKCategories) {
  Document doc;
  google::protobuf::RepeatedPtrField<Category> categories;

  DocumentUtils::GetAndL2NormLevelKCategories(*doc.mutable_category(), 1,
                                              &categories);
  EXPECT_EQ(0, categories.size());

  Category* category = doc.add_category();
  category->set_id(10101);
  category->set_weight(1.0);
  category = doc.add_category();
  category->set_id(10201);
  category->set_weight(2.0);
  category = doc.add_category();
  category->set_id(20101);
  category->set_weight(3.0);
  category = doc.add_category();
  category->set_id(20201);
  category->set_weight(4.0);
  category = doc.add_category();
  category->set_id(30301);
  category->set_weight(5.0);

  categories.Clear();
  EXPECT_EQ(0, categories.size());
  DocumentUtils::GetAndL2NormLevelKCategories(
      *doc.mutable_category(), 1, &categories);
  EXPECT_EQ(3, categories.size());
  EXPECT_EQ(1u, categories.Get(0).id());
  EXPECT_NEAR(0.32929277, categories.Get(0).weight(), kEpsilon);
  EXPECT_EQ(2u, categories.Get(1).id());
  EXPECT_NEAR(0.76834982, categories.Get(1).weight(), kEpsilon);
  EXPECT_EQ(3u, categories.Get(2).id());
  EXPECT_NEAR(0.54882127, categories.Get(2).weight(), kEpsilon);

  categories.Clear();
  EXPECT_EQ(0, categories.size());
  DocumentUtils::GetAndL2NormLevelKCategories(
      *doc.mutable_category(), 2, &categories);
  EXPECT_EQ(5, categories.size());
  EXPECT_EQ(101u, categories.Get(0).id());
  EXPECT_NEAR(0.13483997, categories.Get(0).weight(), kEpsilon);
  EXPECT_EQ(102u, categories.Get(1).id());
  EXPECT_NEAR(0.26967993, categories.Get(1).weight(), kEpsilon);
  EXPECT_EQ(201u, categories.Get(2).id());
  EXPECT_NEAR(0.40451992, categories.Get(2).weight(), kEpsilon);

  categories.Clear();
  EXPECT_EQ(0, categories.size());
  DocumentUtils::GetAndL2NormLevelKCategories(*doc.mutable_category(), 3,
                                              &categories);
  EXPECT_EQ(5, categories.size());
  EXPECT_EQ(10101u, categories.Get(0).id());
  EXPECT_NEAR(0.13483997, categories.Get(0).weight(), kEpsilon);
  EXPECT_EQ(10201u, categories.Get(1).id());
  EXPECT_NEAR(0.26967993, categories.Get(1).weight(), kEpsilon);
  EXPECT_EQ(20101u, categories.Get(2).id());
  EXPECT_NEAR(0.40451992, categories.Get(2).weight(), kEpsilon);
}

}  // namespace text_analysis
}  // namespace qzap
