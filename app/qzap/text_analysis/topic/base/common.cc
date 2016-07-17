// Copyright 2010 Tencent Inc.
// Author: Zhihui Jin (rickjin@tencent.com)
//         Yi Wang (yiwang@tencent.com)
//         Huan Yu (huanyu@tencent.com)

#include "app/qzap/text_analysis/topic/base/common.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/text_analysis/topic/base/cgo_types.h"
#include "app/qzap/text_analysis/topic/base/city.h"
#include "app/qzap/text_analysis/topic/base/lda.pb.h"
#include "app/qzap/text_analysis/topic/base/random.h"

namespace qzap {
namespace text_analysis {
namespace base {

using std::tr1::unordered_map;
using std::string;
using std::vector;

const char* kWordSeparator = "\t";

bool CompareByProb(const std::pair<int32_t, double>& arg1,
                   const std::pair<int32_t, double>& arg2) {
  return arg1.second > arg2.second;
}

bool CompareById(const std::pair<int32_t, double>& arg1,
                 const std::pair<int32_t, double>& arg2) {
  return arg1.first < arg2.first;
}

bool CompareByWordProb(const std::pair<std::string, double>& arg1,
                       const std::pair<std::string, double>& arg2) {
  return arg1.second > arg2.second;
}

int32_t GenerateRandomSeed(const std::vector<std::string>& doc_tokens) {
  std::string doc_str;
  for (size_t i = 0; i < doc_tokens.size(); ++i) {
    doc_str += doc_tokens[i] + "\t";
  }
  int32_t seed = static_cast<int32_t>(
      CityHash32(doc_str.c_str(), doc_str.length()));
  if (seed < 0) {
    return -seed;
  }
  return seed;
}

void UnifyDistribution(std::vector<double>* vec) {
  if (vec->size() == 0)
    return;

  double sum = std::accumulate(vec->begin(), vec->end(), 0.0,
                               std::plus<double>());
  if (sum > 0.0) {
    for (size_t i = 0; i < vec->size(); ++i) {
      (*vec)[i] /= sum;
    }
  }
}

void UnifyDistributionPB(SparseTopicDistributionPB* result) {
  double sum = 0.0;
  for (int i = 0; i < result->vector().nonzero_size(); ++i) {
    sum += result->vector().nonzero(i).value();
  }
  sum += result->shift() * result->num_topics();
  if (sum > 0.0) {
    for (int i = 0; i < result->vector().nonzero_size(); ++i) {
      result->mutable_vector()->mutable_nonzero(i)->set_value(
          result->vector().nonzero(i).value() / sum);
    }
    result->set_shift(result->shift() / sum);
  }
}

void VectorToSparseTopicDistributionPB(
    const std::vector<double>& vec,
    double lda_topic_prior,
    SparseTopicDistributionPB* result) {
  result->set_shift(lda_topic_prior);
  result->set_num_topics(vec.size());
  for (size_t i = 0; i < vec.size(); ++i) {
    if ((vec[i] - lda_topic_prior) >= 1E-6) {
      SparseDoubleVectorPB_NonZero* nonzero =
          result->mutable_vector()->add_nonzero();
      nonzero->set_index(i);
      nonzero->set_value(vec[i] - lda_topic_prior);
    }
  }
}

void SparseTopicDistributionPBToVector(
    const SparseTopicDistributionPB& result, std::vector<double>* vec) {
  vec->resize(result.num_topics(), 0);
  for (int i = 0; i < result.vector().nonzero_size(); ++i) {
    (*vec)[result.vector().nonzero(i).index()] =
        result.vector().nonzero(i).value() + result.shift();
  }
}

int32_t GetAccumulativeSample(const DoubleVector& distribution,
                              Random* random) {
  double distribution_sum = 0.0;
  for (size_t i = 0; i < distribution.size(); ++i) {
    distribution_sum += distribution[i];
  }

  double choice = random->RandDouble() * distribution_sum;
  double sum_so_far = 0.0;
  for (size_t i = 0; i < distribution.size(); ++i) {
    sum_so_far += distribution[i];
    if (sum_so_far >= choice) {
      return static_cast<int32_t>(i);
    }
  }

  LOG(FATAL) << "Failed to choose element from distribution of size "
      << distribution.size() << " and sum " << distribution_sum;

  return -1;
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
