// Copyright 2010 Tencent Inc.
// Author: Zhihui Jin (rickjin@tencent.com)
//
#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_COMMON_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_COMMON_H_

#include <stdint.h>

#include <string>
#include <tr1/unordered_map>
#include <utility>
#include <vector>

namespace qzap {
namespace text_analysis {
namespace base {

class Random;
class SparseTopicDistributionPB;

typedef std::vector<double> DoubleVector;
typedef std::tr1::unordered_map<int32_t, DoubleVector> DoubleMatrix;

typedef std::vector<std::pair<int32_t, double> > SparseDoubleVector;
typedef std::vector<SparseDoubleVector> SparseDoubleMatrix;

typedef std::vector<std::pair<std::string, double> > SparseStringVector;
typedef std::vector<SparseStringVector> SparseStringMatrix;

extern const char* kWordSeparator;

bool CompareByProb(
    const std::pair<int32_t/* id */, double/* prob */>& arg1,
    const std::pair<int32_t, double>& arg2);

bool CompareById(
    const std::pair<int32_t/* id */, double/* prob */>& arg1,
    const std::pair<int32_t, double>& arg2);

bool CompareByWordProb(
    const std::pair<std::string/* word */, double/* prob */>& arg1,
    const std::pair<std::string, double>& arg2);

// Generate seed for Random based on doc_tokens.
int32_t GenerateRandomSeed(const std::vector<std::string>& doc_tokens);

// Utility function for converting count distribution to probability
// distribution
void UnifyDistribution(std::vector<double>* vec);
void UnifyDistributionPB(SparseTopicDistributionPB* result);

// Translate dense vector of count distribution to sparse proto buffer format.
void VectorToSparseTopicDistributionPB(
    const std::vector<double>& vec, double lda_topic_prior,
    SparseTopicDistributionPB* result);

void SparseTopicDistributionPBToVector(
    const SparseTopicDistributionPB& result, std::vector<double>* vec);

int32_t GetAccumulativeSample(const DoubleVector& distribution,
                              Random* random);

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_COMMON_H_
