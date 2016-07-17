// Copyright (c) 2012 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/document_utils.h"

#include <algorithm>

#include "thirdparty/gflags/gflags.h"
#include "app/qzap/text_analysis/classifier/taxonomy_hierarchy.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

DEFINE_string(taxonomy_file, "testdata/taxonomy", "the taxonomy filename");

namespace qzap {
namespace text_analysis {

using google::protobuf::RepeatedPtrField;

void DocumentUtils::GetAndL2NormLevelKCategories(
    const google::protobuf::RepeatedPtrField<Category>& categories,
    int32_t levelk,
    google::protobuf::RepeatedPtrField<Category>* levelk_categories) {
  GetLevelKCategories(categories, levelk, levelk_categories);
  L2Normalize(levelk_categories);
}

void DocumentUtils::GetLevelKCategories(
    const google::protobuf::RepeatedPtrField<Category>& categories,
    int32_t levelk,
    google::protobuf::RepeatedPtrField<Category>* levelk_categories) {
  levelk_categories->Clear();
  if (categories.size() == 0) { return; }

  TaxonomyHierarchy* taxonomy_ = GetTaxonomyPtr();
  int32_t check_depth = taxonomy_->Depth(categories.Get(0).id());
  if (check_depth == levelk) {
    for (int i = 0; i < categories.size(); ++i) {
      Category* category = levelk_categories->Add();
      category->CopyFrom(categories.Get(i));
    }
    return;
  }
  if (check_depth < levelk) {
    LOG(WARNING) << "the categories's depth is less than " << levelk;
    return;
  }
  if (check_depth > taxonomy_->Depth()) {
    LOG(WARNING) << "the categories's depth is more than taxonomy's depth: "
        << taxonomy_->Depth();
    return;
  }

  int num_categories = categories.size();
  typedef std::map<uint32_t, Category*> LevelKCategoryMap;
  LevelKCategoryMap levelk_category_map;
  for (int i = 0; i < num_categories; ++i) {
    uint32_t category_id = categories.Get(i).id();
    while (taxonomy_->Depth(category_id) > levelk) {
      category_id = taxonomy_->Parent(category_id);
    }

    LevelKCategoryMap::iterator iter = levelk_category_map.find(category_id);
    if (iter == levelk_category_map.end()) {
      Category* levelk_category = levelk_categories->Add();
      levelk_category->CopyFrom(categories.Get(i));
      levelk_category->set_id(category_id);

      levelk_category_map[category_id] = levelk_category;
    } else {
      iter->second->set_weight(
          iter->second->weight() + categories.Get(i).weight());
      iter->second->set_ori_weight(
          iter->second->ori_weight() + categories.Get(i).ori_weight());
    }
  }
}

TaxonomyHierarchy* DocumentUtils::GetTaxonomyPtr() {
  static TaxonomyHierarchy taxonomy(FLAGS_taxonomy_file);
  return &taxonomy;
}

}  // namespace text_analysis
}  // namespace qzap

