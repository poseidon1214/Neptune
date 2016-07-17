// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/classifier/training/train_hierarchical_classifier.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/classifier/training/train_maxent.h"

namespace qzap {
namespace text_analysis {

using std::string;
using std::vector;
using std::tr1::unordered_map;
using std::tr1::shared_ptr;

TrainHierarchicalClassifier::TrainHierarchicalClassifier() {}

TrainHierarchicalClassifier::~TrainHierarchicalClassifier() {}

void TrainHierarchicalClassifier::SaveToDir(const std::string& dir) const {
  CHECK(!dir.empty());
  string dirname = dir + "/";
  if (access(dirname.c_str(), R_OK) != 0) {
    CHECK_EQ(0, mkdir(dirname.c_str(), 0777));
  }
  taxonomy_.SaveToTextFile(dirname + "taxonomy");

  unordered_map<int32_t, shared_ptr<TrainClassifierBase> >::const_iterator
      citer = classifiers_.begin();
  while (citer != classifiers_.end()) {
    citer->second->SaveToDir(dirname + ConvertToString(citer->first));
    ++citer;
  }
}

void TrainHierarchicalClassifier::Train(const std::string& sample_filepath,
                                        const TaxonomyHierarchy& taxonomy) {
  CHECK_GE(taxonomy.Depth(), 1);
  classifiers_.clear();
  taxonomy_ = taxonomy;
  TrainAux(sample_filepath, taxonomy_.Root());
}

void TrainHierarchicalClassifier::TrainAux(const std::string& sample_filepath,
                                           int32_t category_id) {
  // get subtree taxonomy of category 'category_id'
  TaxonomyHierarchy sub_taxonomy;
  taxonomy_.SubTreeTaxonomy(category_id, &sub_taxonomy);

  // stop of recursive calling
  if (sub_taxonomy.Depth() < 1) {
    return;
  }
  VLOG(5) << "training node-classifier: " << taxonomy_.Name(category_id)
      << "(" << category_id << ") ...";

  // get taxonomy of node-classifier
  TaxonomyHierarchy flat_taxonomy;
  sub_taxonomy.LayerTaxonomy(1, &flat_taxonomy);

  // training of node-classifier
  TrainClassifierBase* node_classifier = new TrainMaxEnt;
  node_classifier->Train(sample_filepath, flat_taxonomy);
  classifiers_[category_id].reset(node_classifier);

  // recursive calling
  const vector<int32_t>& children = taxonomy_.Children(category_id);
  for (size_t i = 0; i < children.size(); ++i) {
    TrainAux(sample_filepath, children[i]);
  }
}

}  // namespace text_analysis
}  // namespace qzap

