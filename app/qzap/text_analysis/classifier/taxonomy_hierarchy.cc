// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/taxonomy_hierarchy.h"

#include <algorithm>
#include <fstream>  // NOLINT
#include <sstream>  // NOLINT

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"

namespace qzap {
namespace text_analysis {

using std::string;
using std::vector;
using std::tr1::unordered_map;

bool TaxonomyHierarchy::LoadFromTextFile(const std::string& filepath) {
  Clear();

  std::ifstream fin(filepath.c_str());
  if (fin.fail()) {
    LOG(ERROR) << "failed to open file '" << filepath << "'";
    return false;
  }

  string line;
  int32_t line_id = 0;
  while (std::getline(fin, line)) {
    ++line_id;
    if (line.empty() || line[0] == '#') {
      continue;
    }

    vector<string> fields;
    SplitString(line, "\t", &fields);
    if (fields.size() > 3) {
      int32_t id, parentid;
      CHECK(StringToNumeric(fields[0], &id));
      CHECK(StringToNumeric(fields[2], &parentid));
      const string& name = fields[1];

      CHECK(name_to_index_.find(name) == name_to_index_.end());
      CHECK(index_to_node_.find(id) == index_to_node_.end());
      name_to_index_[name] = id;
      if (parentid == -1) {
        root_ = id;
        index_to_node_[id].depth = 0;
      } else {
        CHECK(index_to_node_.find(parentid) != index_to_node_.end());
        index_to_node_[parentid].children.push_back(id);
        index_to_node_[id].depth = index_to_node_[parentid].depth + 1;
      }
      index_to_node_[id].name = name;
      index_to_node_[id].parent = parentid;

      if (index_to_node_[id].depth > depth_) {
        depth_ = index_to_node_[id].depth;
      }
    } else {
      LOG(WARNING) << "line " << line_id << ": \"" << line
          << "\", has not enough fields";
    }
  }
  CHECK_EQ(index_to_node_.size(), name_to_index_.size());
  CHECK(!index_to_node_.empty());
  fin.close();

  return true;
}

void TaxonomyHierarchy::SaveToTextFile(const std::string& filepath) const {
  std::vector<int32_t> ids;
  Descendants(root_, &ids);

  std::ofstream fout(filepath.c_str());
  if (fout.fail()) {
    LOG(ERROR) << "Save file " << filepath << "failed.";
    return;
  }

  for (size_t i = 0; i < ids.size(); ++i) {
    std::ostringstream o;
    const Node& node = index_to_node_.find(ids[i])->second;
    o << ids[i] << '\t' << node.name << '\t'
        << node.parent << '\t' << node.depth << std::endl;
    fout.write(o.str().data(), o.str().length());
  }
  fout.close();
}

bool TaxonomyHierarchy::Has(int32_t id, bool include_root) const {
  std::tr1::unordered_map<int32_t, Node>::const_iterator i =
      index_to_node_.find(id);
  if (include_root) {
    return i != index_to_node_.end();
  }
  return id != root_ && i != index_to_node_.end();
}

bool TaxonomyHierarchy::Has(const std::string& name, bool include_root) const {
  std::tr1::unordered_map<std::string, int32_t>::const_iterator i =
      name_to_index_.find(name);
  if (include_root) {
    return i != name_to_index_.end();
  }
  // not include root
  if (i == name_to_index_.end()) {
    return false;
  }
  return root_ != Id(name);
}

void TaxonomyHierarchy::Ancestors(int32_t id,
                                  std::vector<int32_t>* ancestors) const {
  ancestors->clear();
  while (Has(id, true)) {
    ancestors->push_back(id);
    id = Parent(id);
  }
  std::reverse(ancestors->begin(), ancestors->end());
}

void TaxonomyHierarchy::LayerTaxonomy(
    int32_t depth, TaxonomyHierarchy* sub_taxonomy) const {
  sub_taxonomy->Clear();
  if (depth < 0 || depth > Depth()) {
    return;
  }

  sub_taxonomy->root_ = root_;
  sub_taxonomy->name_to_index_[Name(root_)] = root_;
  sub_taxonomy->index_to_node_[root_].name = Name(root_);
  sub_taxonomy->index_to_node_[root_].parent = -1;
  sub_taxonomy->index_to_node_[root_].depth = 0;
  if (depth == 0) {
    sub_taxonomy->depth_ = 0;
  } else {
    sub_taxonomy->depth_ = 1;
    typedef std::tr1::unordered_map<int32_t, Node>::const_iterator CI;
    int32_t node_count = 0;
    for (CI i = index_to_node_.begin(); i != index_to_node_.end(); ++i) {
      if (i->second.depth == depth) {
        sub_taxonomy->name_to_index_[i->second.name] = i->first;
        sub_taxonomy->index_to_node_[i->first].name = i->second.name;
        sub_taxonomy->index_to_node_[i->first].parent = root_;
        sub_taxonomy->index_to_node_[i->first].depth = 1;
        sub_taxonomy->index_to_node_[root_].children.push_back(i->first);
        ++node_count;
      }
    }
    CHECK_EQ(node_count + 1, sub_taxonomy->NumNodes());
  }
}

void TaxonomyHierarchy::LeafTaxonomy(TaxonomyHierarchy* sub_taxonomy) const {
  sub_taxonomy->Clear();
  if (depth_ < 0) {
    return;
  }

  sub_taxonomy->root_ = root_;
  sub_taxonomy->name_to_index_[Name(root_)] = root_;
  sub_taxonomy->index_to_node_[root_].name = Name(root_);
  sub_taxonomy->index_to_node_[root_].parent = -1;
  sub_taxonomy->index_to_node_[root_].depth = 0;
  if (depth_ == 0) {
    sub_taxonomy->depth_ = 0;
  } else {
    sub_taxonomy->depth_ = 1;
    typedef std::tr1::unordered_map<int32_t, Node>::const_iterator CI;
    int32_t node_count = 0;
    for (CI i = index_to_node_.begin(); i != index_to_node_.end(); ++i) {
      if (i->second.children.empty()) {
        sub_taxonomy->name_to_index_[i->second.name] = i->first;
        sub_taxonomy->index_to_node_[i->first].name = i->second.name;
        sub_taxonomy->index_to_node_[i->first].parent = root_;
        sub_taxonomy->index_to_node_[i->first].depth = 1;
        sub_taxonomy->index_to_node_[root_].children.push_back(i->first);
        ++node_count;
      }
    }
    CHECK_EQ(node_count + 1, sub_taxonomy->NumNodes());
  }
}

void TaxonomyHierarchy::DescendantsAux(
    int32_t id, std::vector<int32_t>* descendants) const {
  if (Has(id, true)) {
    descendants->push_back(id);
    const std::vector<int32_t>& children =
        index_to_node_.find(id)->second.children;
    for (size_t i = 0; i < children.size(); ++i) {
      DescendantsAux(children[i], descendants);
    }
  }
}

void TaxonomyHierarchy::SubTreeTaxonomyAux(
    int32_t id, TaxonomyHierarchy* sub_taxonomy) const {
  if (Has(id, true)) {
    const Node& node = index_to_node_.find(id)->second;
    sub_taxonomy->name_to_index_[node.name] = id;
    sub_taxonomy->index_to_node_[id].name = node.name;
    if (sub_taxonomy->Empty()) {
      sub_taxonomy->root_ = id;
      sub_taxonomy->depth_ = 0;
      sub_taxonomy->index_to_node_[id].depth = 0;
      sub_taxonomy->index_to_node_[id].parent = -1;
    } else {
      sub_taxonomy->index_to_node_[id].parent = node.parent;
      sub_taxonomy->index_to_node_[node.parent].children.push_back(id);
      sub_taxonomy->index_to_node_[id].depth =
          sub_taxonomy->index_to_node_[node.parent].depth + 1;
      if (sub_taxonomy->index_to_node_[id].depth > sub_taxonomy->depth_) {
        sub_taxonomy->depth_ = sub_taxonomy->index_to_node_[id].depth;
      }
    }
    for (size_t i = 0; i < node.children.size(); ++i) {
      SubTreeTaxonomyAux(node.children[i], sub_taxonomy);
    }
  }
}

}  // namespace text_analysis
}  // namespace qzap
