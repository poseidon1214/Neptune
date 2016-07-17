// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/maxent.h"

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/thirdparty/maxent/maxent.h"

namespace qzap {
namespace text_analysis {

MaxEnt::MaxEnt() : impl_(new MaxEntImpl) {}

MaxEnt::~MaxEnt() {
  delete impl_;
  impl_ = NULL;
}

bool MaxEnt::LoadFromDir(const std::string& dir) {
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

  impl_->clear();
  ok = impl_->load_from_file(dirname + "maxent");
  if (!ok) {
    LOG(ERROR) << "failed to load ME_Model";
    return false;
  }

  if (taxonomy_.NumNodes() != impl_->num_classes() + 1) {
    LOG(WARNING) << "MaxEnt " << taxonomy_.Name(taxonomy_.Root())
        << ": taxonomy " << taxonomy_.NumNodes() - 1
        << " vs. model " << impl_->num_classes();
  }
  return true;
}

void MaxEnt::Predict(const Instance& instance, Result* result) const {
  result->clear();
  result->push_back(std::vector<Label>());
  if (taxonomy_.NumNodes() == 2) {
    int32_t id = taxonomy_.Children(taxonomy_.Root()).front();
    result->front().push_back(Label());
    result->front().back().set_id(id);
    result->front().back().set_probability(1.0);
    return;
  }

  MaxEntSample sample;
  for (int32_t i = 0; i < static_cast<int32_t>(instance.NumFeatures()); ++i) {
    sample.add_feature(ConvertToString(instance.IdAt(i)), instance.WeightAt(i));
  }

  std::vector<double> prob = impl_->classify(sample);
  for (size_t i = 0; i < prob.size(); ++i) {
    result->front().push_back(Label());
    result->front().back().set_id(
        taxonomy_.Id(impl_->get_class_label(i)));
    result->front().back().set_probability(prob[i]);
  }
  std::sort(result->front().begin(), result->front().end(),
            CategoryProbabilityGreater);
}

}  // namespace text_analysis
}  // namespace qzap
