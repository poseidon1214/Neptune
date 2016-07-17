// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/taxonomy_hierarchy.h"

#include <algorithm>

#include "thirdparty/gtest/gtest.h"

namespace qzap {
namespace text_analysis {

const char* kFilepath = "testdata/test_taxonomy_hierarchy";

bool EqualVector(const std::vector<int32_t>& lhs,
                 const std::vector<int32_t>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  std::vector<int32_t>::const_iterator i = lhs.begin();
  std::vector<int32_t>::const_iterator j = rhs.begin();
  while (i != lhs.end()) {
    if (!(*i == *j)) {
      return false;
    }
    ++i;
    ++j;
  }
  return true;
}

TEST(TaxonomyHierarchyTest, Empty) {
  TaxonomyHierarchy tax;
  EXPECT_TRUE(tax.Empty());
  EXPECT_EQ(-1, tax.Depth());
  EXPECT_EQ(0, tax.NumNodes());
}

TEST(TaxonomyHierarchyTest, Load) {
  TaxonomyHierarchy tax;
  ASSERT_TRUE(tax.LoadFromTextFile(kFilepath));
  EXPECT_FALSE(tax.Empty());
  EXPECT_EQ(2, tax.Depth());
  EXPECT_EQ(116, tax.NumNodes());
}

TEST(TaxonomyHierarchyTest, Save) {
  TaxonomyHierarchy tax(kFilepath);
  EXPECT_FALSE(tax.Empty());
  EXPECT_EQ(2, tax.Depth());
  EXPECT_EQ(116, tax.NumNodes());

  tax.SaveToTextFile("taxonomy_hierarchy");

  ASSERT_TRUE(tax.LoadFromTextFile("taxonomy_hierarchy"));
  EXPECT_FALSE(tax.Empty());
  EXPECT_EQ(2, tax.Depth());
  EXPECT_EQ(116, tax.NumNodes());
}

TEST(TaxonomyHierarchyTest, Has) {
  TaxonomyHierarchy tax(kFilepath);

  // Has
  EXPECT_TRUE(tax.Has(0, true));
  EXPECT_TRUE(tax.Has(tax.NumNodes() - 1, true));
  EXPECT_FALSE(tax.Has(-1, true));
  EXPECT_FALSE(tax.Has(tax.NumNodes(), true));

  EXPECT_TRUE(tax.Has("root", true));
  EXPECT_TRUE(tax.Has("手机/通讯设备", true));
  EXPECT_FALSE(tax.Has("null", true));
  EXPECT_FALSE(tax.Has("设备", true));

  EXPECT_FALSE(tax.Has(0, false));
  EXPECT_TRUE(tax.Has(tax.NumNodes() - 1, false));
  EXPECT_FALSE(tax.Has(-1, false));
  EXPECT_FALSE(tax.Has(tax.NumNodes(), false));

  EXPECT_FALSE(tax.Has("root", false));
  EXPECT_TRUE(tax.Has("手机/通讯设备", false));
  EXPECT_FALSE(tax.Has("null", false));
  EXPECT_FALSE(tax.Has("设备", false));
}

TEST(TaxonomyHierarchyTest, Id) {
  TaxonomyHierarchy tax(kFilepath);

  // Id
  EXPECT_EQ(0, tax.Id("root"));
  EXPECT_EQ(115, tax.Id("节能环保"));
}

TEST(TaxonomyHierarchyTest, Name) {
  TaxonomyHierarchy tax(kFilepath);

  // Name
  EXPECT_EQ("root", tax.Name(0));
  EXPECT_EQ("节能环保", tax.Name(115));
}

TEST(TaxonomyHierarchyTest, Depth) {
  TaxonomyHierarchy tax(kFilepath);

  // Depth
  EXPECT_EQ(0, tax.Depth(0));
  EXPECT_EQ(2, tax.Depth(115));
}

TEST(TaxonomyHierarchyTest, Children) {
  TaxonomyHierarchy tax(kFilepath);

  // Children
  std::vector<int32_t> child_ids;
  for (size_t i = 1; i <= 26; ++i) {
    child_ids.push_back(i);
  }

  EXPECT_TRUE(EqualVector(tax.Children(0), child_ids));

  child_ids.clear();

  EXPECT_TRUE(EqualVector(tax.Children(115), child_ids));
}

TEST(TaxonomyHierarchyTest, NumChildren) {
  TaxonomyHierarchy tax(kFilepath);

  // NumChildren
  EXPECT_EQ(26, tax.NumChildren(0));
  EXPECT_EQ(0, tax.NumChildren(115));
}

TEST(TaxonomyHierarchyTest, Parent) {
  TaxonomyHierarchy tax(kFilepath);

  // parent
  EXPECT_EQ(-1, tax.Parent(0));
  EXPECT_EQ(26, tax.Parent(115));
}

TEST(TaxonomyHierarchyTest, Ancestors) {
  TaxonomyHierarchy tax(kFilepath);

  // Ancestors
  std::vector<int32_t> ancestors;
  tax.Ancestors(115, &ancestors);
  EXPECT_EQ(3u, ancestors.size());
  EXPECT_EQ(0, ancestors[0]);
  EXPECT_EQ(26, ancestors[1]);
  EXPECT_EQ(115, ancestors[2]);

  tax.Ancestors(116, &ancestors);
  EXPECT_EQ(0u, ancestors.size());
}

TEST(TaxonomyHierarchyTest, Descendants) {
  TaxonomyHierarchy tax(kFilepath);

  // descendants
  std::vector<int32_t> desc_1, desc_2;
  tax.Descendants(-1, &desc_2);
  EXPECT_TRUE(EqualVector(desc_1, desc_2));

  for (int32_t i = 0; i < 116; ++i) {
    desc_1.push_back(i);
  }
  tax.Descendants(0, &desc_2);
  std::sort(desc_2.begin(), desc_2.end());
  EXPECT_TRUE(EqualVector(desc_1, desc_2));
}

TEST(TaxonomyHierarchyTest, SubTaxonomy) {
  TaxonomyHierarchy tax(kFilepath);

  // sub taxonomy
  TaxonomyHierarchy sub;
  tax.SubTreeTaxonomy(0, &sub);
  EXPECT_EQ(tax.NumNodes(), sub.NumNodes());
  tax.SubTreeTaxonomy(115, &sub);
  EXPECT_EQ(1, sub.NumNodes());

  // layer taxonomy
  tax.LayerTaxonomy(-1, &sub);
  EXPECT_TRUE(sub.Empty());
  tax.LayerTaxonomy(0, &sub);
  EXPECT_EQ(1, sub.NumNodes());
  tax.LayerTaxonomy(1, &sub);
  EXPECT_EQ(27, sub.NumNodes());
  tax.LayerTaxonomy(2, &sub);
  EXPECT_EQ(90, sub.NumNodes());

  // leaf taxonomy
  tax.LeafTaxonomy(&sub);
  EXPECT_EQ(95, sub.NumNodes());
}

}  // namespace classifier
}  // namespace paralgo
