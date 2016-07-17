// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/training/train_hierarchical_classifier.h"
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
class TrainHierarchicalClassifierTest : public ::testing::Test {
 public:
  TrainHierarchicalClassifierTest() {}

  virtual ~TrainHierarchicalClassifierTest() {}

  virtual void SetUp() {
    ASSERT_TRUE(taxonomy_regular_.LoadFromTextFile(kTaxonomyFile));
    ASSERT_EQ(6, taxonomy_regular_.NumNodes());
    taxonomy_regular_.SubTreeTaxonomy(4, &taxonomy_2levels_);
    ASSERT_EQ(2, taxonomy_2levels_.NumNodes());
  }

  virtual void TearDown() {}

  TaxonomyHierarchy taxonomy_regular_;
  TaxonomyHierarchy taxonomy_2levels_;
  TrainHierarchicalClassifier hierarchical_classifier_trainer_;
};  // class TrainHierarchicalClassifierTest

TEST_F(TrainHierarchicalClassifierTest, TrainTaxonomy2Levels) {
  const TaxonomyHierarchy* taxonomy;
  taxonomy = &hierarchical_classifier_trainer_.taxonomy();

  EXPECT_TRUE(taxonomy->Empty());
  hierarchical_classifier_trainer_.Train(kSampleFile, taxonomy_2levels_);
  EXPECT_EQ(2, taxonomy->NumNodes());
}

TEST_F(TrainHierarchicalClassifierTest, TrainTaxonomyRegular) {
  const TaxonomyHierarchy* taxonomy;
  taxonomy = &hierarchical_classifier_trainer_.taxonomy();

  EXPECT_TRUE(taxonomy->Empty());
  hierarchical_classifier_trainer_.Train(kSampleFile, taxonomy_regular_);
  EXPECT_EQ(6, taxonomy->NumNodes());
}

TEST_F(TrainHierarchicalClassifierTest, SaveTaxonomy2LevelsModel) {
  hierarchical_classifier_trainer_.Train(kSampleFile, taxonomy_2levels_);
  hierarchical_classifier_trainer_.SaveToDir("./model_taxonomy_2levels");
}

TEST_F(TrainHierarchicalClassifierTest, SaveTaxonomyRegularModel) {
  hierarchical_classifier_trainer_.Train(kSampleFile, taxonomy_regular_);
  hierarchical_classifier_trainer_.SaveToDir("./model_taxonomy_regular");
}

}  // namespace text_analysis
}  // namespace qzap

