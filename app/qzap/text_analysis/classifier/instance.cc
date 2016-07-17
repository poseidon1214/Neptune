// Copyright (c) 2011 Tencent Inc.
// Author: Zhiqiang Chen (lucienchen@tencent.com)

#include "app/qzap/text_analysis/classifier/instance.h"

#include <math.h>
#include <algorithm>

#include "app/qzap/common/base/string_utility.h"
#include "thirdparty/glog/logging.h"

namespace qzap {
namespace text_analysis {

double Instance::L1Norm() const {
  double sum = 0;
  for (ConstDataIter i = data_.begin(); i != data_.end(); ++i) {
    sum += i->second;
  }
  return sum;
}

void Instance::L1Normalize() {
  double l1_norm = L1Norm();
  if (l1_norm > 0) {
    for (DataIter i = data_.begin(); i != data_.end(); ++i) {
      i->second /= l1_norm;
    }
  } else {
    VLOG(5) << "L1 norm is equal 0";
  }

}

double Instance::L2Norm() const {
  double sum = 0;
  for (ConstDataIter i = data_.begin(); i != data_.end(); ++i) {
    sum += i->second * i->second;
  }
  return sqrt(sum);
}

void Instance::L2Normalize() {
  double l2_norm = L2Norm();
  if (l2_norm > 0) {
    for (DataIter i = data_.begin(); i != data_.end(); ++i) {
      i->second /= l2_norm;
    }
  } else {
    VLOG(5) << "L2 norm is equal 0";
  }
}

void Instance::AddFeature(uint32_t id, double weight) {
  data_.push_back(std::pair<uint32_t, double>(id, weight));
}

void Instance::ParseFrom(const std::string& str) {
  data_.clear();
  std::vector<std::string> tokens;
  SplitString(str, " ", &tokens);
  if (tokens.empty()) {
    return;
  }

  for(size_t i = 0; i < tokens.size(); ++i) {
    std::vector<std::string> tmp;
    SplitString(tokens[i], ":", &tmp);
    if (tmp.size() != 2) {
      LOG(WARNING) << "the record is invalid!";
      continue;
    }
    uint32_t id = 0;
    double weight = 0.0;
    StringToNumeric(tmp[0], &id);
    StringToNumeric(tmp[1], &weight);
    data_.push_back(std::pair<uint32_t, double>(id, weight));
  }
}

void Instance::SerializeTo(std::string* str) const {
  for (size_t i = 0; i < data_.size(); ++i) {
    std::string tmp;
    StringAppendF(&tmp, "%d:%lf ", data_[i].first, data_[i].second);
    str->append(tmp);
  }
  TrimString(str);
}

static bool CompareWeightGreater(const std::pair<uint32_t, double>& l,
                                 const std::pair<uint32_t, double>& r) {
  return l.second > r.second;
}

void Instance::SortById() { std::sort(data_.begin(), data_.end()); }

void Instance::SortByWeight() {
  std::sort(data_.begin(), data_.end(), &CompareWeightGreater);
}

void Instance::Clear() { data_.clear(); }

bool Instance::Empty() { return data_.empty(); }

}  // namespace text_analysis
}  // namespace qzap

