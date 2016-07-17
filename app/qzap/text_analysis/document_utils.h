// Copyright (c) 2012 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// utilities for proto msg Document

#ifndef APP_QZAP_TEXT_ANALYSIS_DOCUMENT_UTILS_H_
#define APP_QZAP_TEXT_ANALYSIS_DOCUMENT_UTILS_H_

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include <math.h>
#include <algorithm>
#include <functional>

#include "app/qzap/common/base/scoped_ptr.h"
#include "thirdparty/protobuf/repeated_field.h"

DECLARE_string(taxonomy_file);

namespace qzap {
namespace text_analysis {

class Token;
class Topic;
class Category;
class Keyword;
class Document;
class TaxonomyHierarchy;

// Functor template comparing the weight() property.
template <typename MessageType>
struct WeightGreater
    : public std::binary_function<MessageType, MessageType, bool> {
  bool operator()(const MessageType& lhs, const MessageType& rhs) const {
    return lhs.weight() > rhs.weight();
  }
};  // struct WeightGreater

template <typename MessageType>
struct SignatureComparator {
  bool operator()(const MessageType& lhs, const MessageType& rhs) {
    return lhs.signature() < rhs.signature();
  }
};  // struct TokenComparator

class DocumentUtils {
 public:
  template<class MessageType>
  static void L1Normalize(
      google::protobuf::RepeatedPtrField<MessageType>* features);

  template<class MessageType>
  static void L2Normalize(
      google::protobuf::RepeatedPtrField<MessageType>* features);

  template<class MessageType>
  static void TruncateRepeatedField(
      const int32_t top_k,
      google::protobuf::RepeatedPtrField<MessageType>* features);

  template<class MessageType>
  static void TruncateRepeatedField(
      const double threshold,
      google::protobuf::RepeatedPtrField<MessageType>* features);

  template<class MessageType>
  static void TruncateRepeatedField(
      const int32_t top_k,
      const double threshold,
      google::protobuf::RepeatedPtrField<MessageType>* features);

  template<class MessageType>
  static void TruncateAndL2NormRepeatedField(
      const int32_t top_k,
      google::protobuf::RepeatedPtrField<MessageType>* features);

  template<class MessageType>
  static void TruncateAndL2NormRepeatedField(
      const double threshold,
      google::protobuf::RepeatedPtrField<MessageType>* features);

  template<class MessageType>
  static void TruncateAndL2NormRepeatedField(
      const int32_t top_k,
      const double threshold,
      google::protobuf::RepeatedPtrField<MessageType>* features);

  static void GetSortedTokens(
      const Document& doc,
      google::protobuf::RepeatedPtrField<Token>* tokens);

  static void GetAndL2NormLevelKCategories(
      const google::protobuf::RepeatedPtrField<Category>& categories,
      int32_t levelk,
      google::protobuf::RepeatedPtrField<Category>* levelk_categories);

  static void GetLevelKCategories(
      const google::protobuf::RepeatedPtrField<Category>& categories,
      int32_t levelk,
      google::protobuf::RepeatedPtrField<Category>* levelk_categories);

  static TaxonomyHierarchy* GetTaxonomyPtr();
};  // class DocumentUtils

template<typename MessageType>
void DocumentUtils::L1Normalize(
    google::protobuf::RepeatedPtrField<MessageType>* features) {
  double sum = 0.0;
  for (int i = 0; i < features->size(); ++i) {
    sum += features->Get(i).weight();
  }

  static double kEpsilon = 1E-8;
  if (sum < kEpsilon) { return; }
  for (int i = 0; i < features->size(); ++i) {
    MessageType* feature = features->Mutable(i);
    feature->set_weight(feature->weight() / sum);
  }
}

template<typename MessageType>
void DocumentUtils::L2Normalize(
    google::protobuf::RepeatedPtrField<MessageType>* features) {
  double sum = 0.0;
  for (int i = 0; i < features->size(); ++i) {
    sum += features->Get(i).weight() * features->Get(i).weight();
  }
  sum = sqrt(sum);

  static double kEpsilon = 1E-8;
  if (sum < kEpsilon) {
    return;
  }
  for (int i = 0; i < features->size(); ++i) {
    MessageType* feature = features->Mutable(i);
    feature->set_weight(feature->weight() / sum);
  }
}

template<class MessageType>
void DocumentUtils::TruncateRepeatedField(
    const int32_t top_k,
    google::protobuf::RepeatedPtrField<MessageType>* features) {
  if (top_k < 0) {
    LOG(WARNING) << "The top_k is negative, top_k: " << top_k;
  }
  int num = features->size();
  if (num > top_k) {
    std::partial_sort(features->begin(), features->begin() + top_k,
                      features->end(), WeightGreater<MessageType>());
    while (num > top_k && num >= 0) {
      features->RemoveLast();
      --num;
    }
  }
}

template<class MessageType>
void DocumentUtils::TruncateRepeatedField(
    const double threshold,
    google::protobuf::RepeatedPtrField<MessageType>* features) {
  std::sort(features->begin(), features->end(), WeightGreater<MessageType>());
  for (int i = features->size() - 1;
       i >= 0 && features->Get(i).weight() < threshold; --i) {
    features->RemoveLast();
  }
}

template<class MessageType>
void DocumentUtils::TruncateRepeatedField(
    const int32_t top_k,
    const double threshold,
    google::protobuf::RepeatedPtrField<MessageType>* features) {
  if (top_k < 0) {
    LOG(WARNING) << "The top_k is negative, top_k: " << top_k;
  }
  int num = features->size();
  int32_t top_k_sort = top_k;
  if (top_k > num) {
    top_k_sort = num;
  }
  std::partial_sort(features->begin(), features->begin() + top_k_sort,
                    features->end(), WeightGreater<MessageType>());
  while (num > top_k && num >= 0) {
    features->RemoveLast();
    --num;
  }
  for (int i = features->size() - 1;
       i >= 0 && features->Get(i).weight() < threshold; --i) {
    features->RemoveLast();
  }
}

template<class MessageType>
void DocumentUtils::TruncateAndL2NormRepeatedField(
    const int32_t top_k,
    google::protobuf::RepeatedPtrField<MessageType>* features) {
  TruncateRepeatedField(top_k, features);
  L2Normalize(features);
}

template<class MessageType>
void DocumentUtils::TruncateAndL2NormRepeatedField(
    const double threshold,
    google::protobuf::RepeatedPtrField<MessageType>* features) {
  TruncateRepeatedField(threshold, features);
  L2Normalize(features);
}

template<class MessageType>
void DocumentUtils::TruncateAndL2NormRepeatedField(
    const int32_t top_k,
    const double threshold,
    google::protobuf::RepeatedPtrField<MessageType>* features) {
  TruncateRepeatedField(top_k, threshold, features);
  L2Normalize(features);
}

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_DOCUMENT_UTILS_H_
