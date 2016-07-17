// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/hierarchical_classifier.h"

#include <algorithm>
#include "thirdparty/gflags/gflags.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/classifier/optim_maxent.h"

DEFINE_double(hierarchical_classifier_threshold, 0.2,
              "threshold for determinating whether this instance "
              "should be classified by deeper classifiers");

namespace qzap {
namespace text_analysis {

bool HierarchicalClassifier::LoadFromDir(const std::string& dir) {
  if (dir.empty()) {
    LOG(ERROR) << "directory path string is empty";
    return false;
  }

  std::string dirname = dir + "/";
  bool ok = taxonomy_.LoadFromTextFile(dirname + "taxonomy");
  if (!ok) {
    LOG(ERROR) << "failed to load taxonomy";
    return false;
  }

  std::vector<int32_t> descendants;
  taxonomy_.Descendants(taxonomy_.Root(), &descendants);
  classifiers_.clear();
  for (size_t i = 0; i < descendants.size(); ++i) {
    if (taxonomy_.NumChildren(descendants[i]) > 0) {
      ClassifierBase* maxent = new OptimMaxEnt;
      classifiers_[descendants[i]].reset(maxent);
      ok = maxent->LoadFromDir(dirname + ConvertToString(descendants[i]));
      if (!ok) {
        LOG(ERROR) << "failed to load classifier "
            << taxonomy_.Name(descendants[i]);
        return false;
      }
    }
  }
  return true;
}

void HierarchicalClassifier::Predict(const Instance& instance,
                                     Result* result) const {
  result->clear();
  PredictAux(instance, taxonomy_.Root(),
             FLAGS_hierarchical_classifier_threshold, result);
}

void HierarchicalClassifier::PredictAux(const Instance& instance,
                                        int32_t id,
                                        double continue_threshold,
                                        Result* result) const {
  // return from recursive call
  ClassifierMap::const_iterator classifier_i = classifiers_.find(id);
  if (classifier_i == classifiers_.end()) { return; }

  // classify current category node using flat classifier
  Result flat_result;
  classifier_i->second->Predict(instance, &flat_result);
  std::vector<Label>& flat_layer_result = flat_result.front();

  // expand result
  int32_t depth = taxonomy_.Depth(id);
  if (static_cast<size_t>(depth) == result->size()) {
    result->push_back(std::vector<Label>());
  }
  std::vector<Label>& curr_layer_result = (*result)[depth];

  // calculate category probability p:
  // p = category-probability * parent-category-probability
  std::sort(flat_layer_result.begin(), flat_layer_result.end(),
            CategoryProbabilityGreater);
  if (depth == 0) {
    curr_layer_result = flat_layer_result;
  } else {
    size_t offset = curr_layer_result.size();
    curr_layer_result.resize(offset + flat_layer_result.size());
    const std::vector<Label>& prev_layer_result = (*result)[depth - 1];

    for (size_t i = 0; i < flat_layer_result.size(); ++i) {
      // find parent-category-probability
      int32_t parent_id = taxonomy_.Parent(flat_layer_result[i].id());
      double parent_prob = -1.0;
      for (size_t j = 0; j < prev_layer_result.size(); ++j) {
        if (prev_layer_result[j].id() == parent_id) {
          parent_prob = prev_layer_result[j].probability();
          break;
        }
      }
      if (parent_prob < 0) {
        LOG(ERROR) << "can't find '" << parent_id << "' in Result";
        continue;
      }

      curr_layer_result[i + offset].set_id(flat_layer_result[i].id());
      curr_layer_result[i + offset].set_probability(
          parent_prob * flat_layer_result[i].probability());
    }
  }

  // recursive call
  for (size_t i = 0; i < flat_layer_result.size(); ++i) {
    // pruning, 排名第1的和大于阈值的都试一下
    if (i == 0 || flat_layer_result[i].probability() > continue_threshold) {
      PredictAux(instance, flat_layer_result[i].id(),
                 continue_threshold, result);
    }
  }
}

}  // namespace text_analysis
}  // namespace qzap
