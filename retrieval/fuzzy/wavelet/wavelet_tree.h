// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#ifndef CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_WAVELET_TREE_H_
#define CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_WAVELET_TREE_H_

#include <utility>
#include <vector>
#include "data_collector/feeder/candidate/similarity/wavelet_matrix.h"

namespace gdt {
namespace dynamic_creative {

class WaveletTree: public wavelet_matrix::WaveletMatrix {
 public:
  // N个Doc至少命中M个
  void Intersect(
    const std::vector<std::pair<size_t, size_t> >& patterns,
    size_t thres,
    size_t max_count,
    std::vector<int64_t>* results);

  // 带权检索
  void Retrieval(
    const std::vector<std::pair<std::pair<size_t, size_t>, double> >& patterns,
    double thres,
    std::vector<int64_t>* results);

  // 带filter的带权检索
  void RetrievalWithFilter(
    const std::vector<std::pair<std::pair<size_t, size_t>, double> >& patterns,
    const std::vector<std::pair<size_t, size_t> >& filter_patterns,
    double thres,
    std::vector<int64_t>* results);

  // TODO(cernwang) 增加最优化剪枝的策略
  void TopKRetrieval(
    const std::vector<std::pair<std::pair<size_t, size_t>, double> >& patterns,
    const std::vector<std::pair<size_t, size_t> >& filter_patterns,
    double thres,
    std::vector<int64_t>* results);

 private:
  void IntersectEachLevel(
    const std::vector<std::pair<size_t, size_t> >& patterns,
    size_t thres,
    size_t max_count,
    size_t level,
    int64_t symbol,
    std::vector<int64_t>* results);

  void RetrievalEachLevel(
    const std::vector<std::pair<std::pair<size_t, size_t>, double> >& patterns,
    double thres,
    size_t level,
    int64_t symbol,
    std::vector<int64_t>* results);

  void RetrievalWithFilterEachLevel(
    const std::vector<std::pair<std::pair<size_t, size_t>, double> >& patterns,
    const std::vector<std::pair<size_t, size_t> >& filter_patterns,
    double thres,
    size_t level,
    int64_t symbol,
    std::vector<int64_t>* results);
};

}  // namespace dynamic_creative
}  // namespace gdt

#endif  // CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_WAVELET_TREE_H_
