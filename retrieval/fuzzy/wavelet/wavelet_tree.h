// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#ifndef CREATIVE_wavelet_CANDIDATE_SIMILARITY_WAVELET_TREE_H_
#define CREATIVE_wavelet_CANDIDATE_SIMILARITY_WAVELET_TREE_H_

#include <utility>
#include <vector>
#include <tuple>
#include "retrieval/fuzzy/wavelet/wavelet_matrix.h"

typedef std::vector<std::tuple<size_t, size_t, double> > FieldPattern;
typedef std::vector<std::pair<FieldPattern, double> > QueryPattern;

namespace gdt {
namespace wavelet {

class WaveletTree: public wavelet_matrix::WaveletMatrix {
 public:
  // 带权检索
  bool Retrieval(
      QueryPattern& query_pattern,
      std::vector<uint64_t>* results);

 private:
  bool RetrievalEachLevel(
      const QueryPattern& query_pattern,
      size_t level,
      uint64_t symbol,
      std::vector<uint64_t>* results);

  void SatisfyCondition(
      const FieldPattern& patterns, double thres,
      const wat_array::BitArray& ba, size_t zero_count,
      std::pair<FieldPattern, double>* zero_result,
      std::pair<FieldPattern, double>* one_result,
      bool& has_zeros, bool& has_ones);
};

}  // namespace wavelet
}  // namespace gdt

#endif  // CREATIVE_wavelet_CANDIDATE_SIMILARITY_WAVELET_TREE_H_
