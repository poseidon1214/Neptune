// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/training/train_maxent.h"
#include "thirdparty/gtest/gtest.h"

const std::string kTaxonomyFile = "testdata/taxonomy";
const std::string kSampleFile = "testdata/training_samples";

namespace qzap {
namespace text_analysis {

// taxonomy hierarchy:                   |
//             root(0)                   |
//           /       \                   |
//        餐饮(1)   时尚(4)              |
//       /      \         \              |
//   餐馆(2)烹饪/菜谱(3) 服饰鞋帽箱包(5) |
class TrainMaxEntTest : public ::testing::Test {
 public:
  TrainMaxEntTest() {}

  virtual ~TrainMaxEntTest() {}

  virtual void SetUp() {
    TaxonomyHierarchy hierarchical_taxonomy;
    ASSERT_TRUE(hierarchical_taxonomy.LoadFromTextFile(kTaxonomyFile));
    hierarchical_taxonomy.LayerTaxonomy(1, &taxonomy_2categories_);
    ASSERT_EQ(3, taxonomy_2categories_.NumNodes());
    hierarchical_taxonomy.SubTreeTaxonomy(4, &taxonomy_1category_);
    ASSERT_EQ(2, taxonomy_1category_.NumNodes());
  }

  virtual void TearDown() {}

  TaxonomyHierarchy taxonomy_2categories_;
  TaxonomyHierarchy taxonomy_1category_;
  TrainMaxEnt maxent_trainer_;
};  // class TrainMaxEntTest

TEST_F(TrainMaxEntTest, Train2Categories) {
  const TaxonomyHierarchy* taxonomy;
  taxonomy = &maxent_trainer_.taxonomy();

  EXPECT_TRUE(taxonomy->Empty());
  maxent_trainer_.Train(kSampleFile, taxonomy_2categories_);
  EXPECT_EQ(3, taxonomy->NumNodes());
}

TEST_F(TrainMaxEntTest, Train1Categories) {
  const TaxonomyHierarchy* taxonomy;
  taxonomy = &maxent_trainer_.taxonomy();

  EXPECT_TRUE(taxonomy->Empty());
  maxent_trainer_.Train(kSampleFile, taxonomy_1category_);
  EXPECT_EQ(2, taxonomy->NumNodes());
}

TEST_F(TrainMaxEntTest, Save2CategoriesModel) {
  maxent_trainer_.Train(kSampleFile, taxonomy_2categories_);
  maxent_trainer_.SaveToDir("./model_2categories");
}

TEST_F(TrainMaxEntTest, Save1CategoryModel) {
  maxent_trainer_.Train(kSampleFile, taxonomy_1category_);
  maxent_trainer_.SaveToDir("./model_1category");
}

}  // namespace text_analysis
}  // namespace qzap
