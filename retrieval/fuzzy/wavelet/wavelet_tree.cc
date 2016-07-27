// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include <utility>
#include <vector>
#include "retrieval/fuzzy/wavelet/wavelet_tree.h"

namespace gdt {
namespace wavelet {

bool WaveletTree::Retrieval(
  QueryPattern& query_pattern,
  std::vector<uint64_t>* results) {
  double sum = 0;
  for (auto field : query_pattern) {
    FieldPattern& field_pattern = field.first;
    double& thres = field.second;
    for (int i = 0; i < field_pattern.size(); i++) {
      sum += std::get<2>( field_pattern[i]);
    }
    if (thres > sum) {
      return true;
    }
    thres = sum - thres;  
  }

  return RetrievalEachLevel(query_pattern, 0, 0, results);
}

bool WaveletTree::RetrievalEachLevel(
  const QueryPattern& query_pattern,
  size_t level,
  uint64_t symbol,
  std::vector<uint64_t>* results) {
  if (level == this->alphabet_bit_num_) {
    results->push_back(symbol);
    return true;
  }

  const wat_array::BitArray& ba = bit_arrays_[level];
  bool has_zeros = true, has_ones = true;
  size_t zero_count = zero_counts_[level];
  QueryPattern zero_query;
  QueryPattern one_query;
  for (auto field : query_pattern) {
    const FieldPattern& field_pattern = field.first;
    double thres = field.second;
    std::pair<FieldPattern, double> zero_result;
    std::pair<FieldPattern, double> one_result;
    SatisfyCondition(field_pattern, thres, ba, zero_count,
                     &zero_result, &one_result, has_zeros, has_ones);
    if (has_zeros) {
      zero_query.push_back(zero_result);
    }
    if (has_ones) {
      one_query.push_back(one_result);
    }
    if (!has_zeros && !has_ones) {
      break;
    }
  }

  if (has_zeros) {
    RetrievalEachLevel(zero_query, level + 1, symbol, results);
  }
  if (has_ones) {
    RetrievalEachLevel(one_query, level + 1,
                       symbol | (uint64_t)1 << (this->alphabet_bit_num_ - level - 1), results);
  }
  return true;
}

void WaveletTree::SatisfyCondition(const FieldPattern& patterns, double thres,
                                   const wat_array::BitArray& ba, size_t zero_count,
                                   std::pair<FieldPattern, double>* zero_result,
                                   std::pair<FieldPattern, double>* one_result,
                                   bool& has_zeros,  bool& has_ones) {
  FieldPattern& zero_ranges = zero_result->first;
  FieldPattern& one_ranges = one_result->first;
  zero_ranges.reserve(patterns.size());
  one_ranges.reserve(patterns.size());
  double& zero_thres = zero_result->second;
  double& one_thres = one_result->second;
  zero_thres = thres;
  one_thres = thres;
  for (auto it = patterns.begin(); it != patterns.end(); ++it) {
    size_t rank_start = ba.Rank(1, std::get<0>(*it));
    size_t rank_end = ba.Rank(1,  std::get<1>(*it));
    if (has_zeros) {
      if (std::get<0>(*it) - rank_start < std::get<1>(*it) - rank_end) {
        zero_ranges.push_back(std::make_tuple(std::get<0>(*it) - rank_start,  std::get<1>(*it) - rank_end,  std::get<2>(*it)));
      } else {
        zero_thres -= std::get<2>(*it);
        if (zero_thres < 0) {
          if (!has_ones) return;
          has_zeros = false;
        }
      }
    }
    if (has_ones) {
      if (rank_start < rank_end) {
        one_ranges.push_back(std::make_tuple(rank_start + zero_count, rank_end + zero_count, std::get<2>(*it)));
      } else {
        one_thres -= std::get<2>(*it);
        if (one_thres < 0) {
          if (!has_zeros) return;
          has_ones = false;
        }
      }
    }
  }
}

}  // namespace wavelet
}  // namespace gdt
