// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TAXONOMY_HIERARCHY_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TAXONOMY_HIERARCHY_H_

#include <string>
#include <tr1/unordered_map>
#include <vector>

#include "thirdparty/glog/logging.h"

namespace qzap {
namespace text_analysis {

// taxonomy hierarchy for hierachical classifer
//
// note: 1. category name in taxonomy hierarchy MUST be different.
//       2. in taxonomy hierarchy text file,
//          parent MUST appear before children.
class TaxonomyHierarchy {
 public:
  TaxonomyHierarchy() : depth_(-1), root_(-1) {}

  explicit TaxonomyHierarchy(const std::string& filepath) {
    CHECK(LoadFromTextFile(filepath));
  }

  ~TaxonomyHierarchy() { Clear(); }

  // taxonomy file format:
  // A line contains infomation about one category,
  // format "category-id\tcategory-name\tparent-id\tdepth"
  // eg.
  //  0\troot\t-1\t0
  //  1\tcat-1\t0\t1
  //  2\tcat-2\t0\t1
  //  3\tcat-3\t1\t2
  // corresponding taxonomy hierarchy
  //             root                 |
  //            /    \                |
  //        cat-1    cat-2            |
  //          |                       |
  //        cat-3                     |
  bool LoadFromTextFile(const std::string& filepath);
  void SaveToTextFile(const std::string& filepath) const;

  bool Empty() const { return depth_ == -1; }

  int32_t Root() const { return root_; }

  int32_t Depth() const { return depth_; }

  int32_t NumNodes() const {
    return static_cast<int32_t>(name_to_index_.size());
  }

  bool Has(int32_t id, bool include_root) const;

  bool Has(const std::string& name, bool include_root) const;

  const std::string& Name(int32_t id) const {
    CHECK(Has(id, true));
    return index_to_node_.find(id)->second.name;
  }

  inline bool GetName(const int32_t& id, std::string* name) const {
    if (!Has(id, true)) {
      return false;
    }
    if (name->assign(index_to_node_.find(id)->second.name).empty()) {
      return false;
    }
    return true;
  }

  int32_t Id(const std::string& name) const {
    CHECK(Has(name, true));
    return name_to_index_.find(name)->second;
  }

  int32_t Depth(int32_t id) const {
    CHECK(Has(id, true));
    return index_to_node_.find(id)->second.depth;
  }

  const std::vector<int32_t>& Children(int32_t id) const {
    CHECK(Has(id, true));
    return index_to_node_.find(id)->second.children;
  }

  int32_t NumChildren(int32_t id) const {
    return static_cast<int32_t>(Children(id).size());
  }

  int32_t Parent(int32_t id) const {
    CHECK(Has(id, true));
    return index_to_node_.find(id)->second.parent;
  }

  // including 'id' itself
  void Ancestors(int32_t id,
                 std::vector<int32_t>* ancestors) const;

  // including 'id' itself
  void Descendants(int32_t id,
                   std::vector<int32_t>* descendants) const {
    descendants->clear();
    DescendantsAux(id, descendants);
  }

  void SubTreeTaxonomy(int32_t id, TaxonomyHierarchy* sub_taxonomy) const {
    sub_taxonomy->Clear();
    SubTreeTaxonomyAux(id, sub_taxonomy);
  }

  // Sub-taxonomy contains nodes at layer 'depth'
  void LayerTaxonomy(int32_t depth, TaxonomyHierarchy* sub_taxonomy) const;

  // Sub-taxonomy contains leaf nodes
  void LeafTaxonomy(TaxonomyHierarchy* sub_taxonomy) const;

 private:
  struct Node {
    std::string name;
    int32_t parent;
    std::vector<int32_t> children;
    int32_t depth;
  };  // struct Node

  void Clear() {
    index_to_node_.clear();
    name_to_index_.clear();
    depth_ = -1;
    root_ = -1;
  }

  void DescendantsAux(int32_t id,
                      std::vector<int32_t>* descendants) const;

  void SubTreeTaxonomyAux(int32_t id, TaxonomyHierarchy* sub_taxonomy) const;

  std::tr1::unordered_map<int32_t, Node> index_to_node_;
  std::tr1::unordered_map<std::string, int32_t> name_to_index_;
  int32_t depth_;
  int32_t root_;
};  // class TaxonomyHierarchy

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TAXONOMY_HIERARCHY_H_
