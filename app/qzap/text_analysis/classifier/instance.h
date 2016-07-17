// Copyright (c) 2011 Tencent Inc.
// Author: ZhiQiang Chen (lucienchen@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_INSTANCE_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_INSTANCE_H_

#include <stdint.h>
#include <string>
#include <vector>
#include "app/qzap/common/base/string_utility.h"

namespace qzap {
namespace text_analysis {

class Instance {
 public:
  Instance() : debug_label_(false) {}
  ~Instance() {}

  double L1Norm() const;
  void L1Normalize();

  double L2Norm() const;
  void L2Normalize();

  //  add feature的时候，要保证id是唯一的，不能重复
  void AddFeature(uint32_t id, double weight);

  uint32_t NumFeatures() const { return data_.size(); }

  uint32_t IdAt(size_t i) const { return data_[i].first; }

  double WeightAt(size_t i) const { return data_[i].second; }

  void ParseFrom(const std::string& str);

  void SerializeTo(std::string* str) const;

  void SortById();

  void SortByWeight();

  void Clear();

  bool Empty();
  
  void AddDebugString(const std::string& str) const {
    StringAppendF(&debug_string_, "%s\n", str.c_str());
    SetDebug(true);
  }
  const std::string& GetDebugString() const {
    return debug_string_;
  }
  bool IsDebug() const {
    return debug_label_;
  }
  void SetDebug(bool is_debug) const {
    debug_label_ = is_debug;
  }
 private:
  typedef
      std::vector<std::pair<uint32_t, double> >::const_iterator ConstDataIter;
  typedef std::vector<std::pair<uint32_t, double> >::iterator DataIter;

  std::vector<std::pair<uint32_t, double> > data_;

  mutable std::string debug_string_;
  mutable bool debug_label_;
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_INSTANCE_H_
