// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include <utility>
#include <vector>
#include "data_collector/feeder/candidate/similarity/wavelet_tree.h"

namespace gdt {
namespace dynamic_creative {

void WaveletTree::Intersect(
  const std::vector<std::pair<size_t, size_t> >& patterns,
  size_t thres,
  size_t max_count,
  std::vector<int64_t>* results) {
  if (thres > patterns.size()) return;
  if (thres > 0) thres = patterns.size() - thres;
  (*results).reserve(max_count);
  IntersectEachLevel(patterns, thres, max_count, 0, 0, results);
}

void WaveletTree::Retrieval(
  const std::vector<std::pair<std::pair<size_t, size_t>, double> >& patterns,
  double thres,
  std::vector<int64_t>* results) {
  double sum = 0;
  for (int i = 0; i < patterns.size(); i++) {
    sum += patterns[i].second;
  }
  if (thres > sum) {
    return;
  }
  if (thres > 0) {
    thres = sum - thres;
  }
  RetrievalEachLevel(patterns, thres, 0, 0, results);
}

void WaveletTree::RetrievalWithFilter(
  const std::vector<std::pair<std::pair<size_t, size_t>, double> >& patterns,
  const std::vector<std::pair<size_t, size_t> >& filter_patterns,
  double thres,
  std::vector<int64_t>* results) {
  double sum = 0;
  for (int i = 0; i < patterns.size(); i++) {
    sum += patterns[i].second;
  }
  if (thres > sum) {
    return;
  }
  if (thres > 0) {
    thres = sum - thres;
  }
  RetrievalWithFilterEachLevel(patterns, filter_patterns,
                               thres, 0, 0, results);
}

void WaveletTree::RetrievalWithFilterEachLevel(
  const std::vector<std::pair<std::pair<size_t, size_t>, double> >& patterns,
  const std::vector<std::pair<size_t, size_t> >& filter_patterns,
  double thres,
  size_t level,
  int64_t symbol,
  std::vector<int64_t>* results) {
  if (level == this->alphabet_bit_num_) {
    results -> push_back(symbol);
    return;
  }
  std::vector<std::pair<std::pair<size_t, size_t>, double> >
  zero_ranges, one_ranges;
  std::vector<std::pair<size_t, size_t> > filter_zero_ranges, filter_one_ranges;

  zero_ranges.reserve(patterns.size());
  one_ranges.reserve(patterns.size());
  filter_zero_ranges.reserve(filter_patterns.size());
  filter_one_ranges.reserve(filter_patterns.size());

  const wat_array::BitArray& ba = bit_arrays_[level];
  size_t zero_count = zero_counts_[level];

  for (std::vector<std::pair<size_t, size_t> >::const_iterator it =
         filter_patterns.begin(); it != filter_patterns.end(); ++it) {
    size_t rank_start = ba.Rank(1, it->first);
    size_t rank_end = ba.Rank(1, it->second);

    if (it->first - rank_start < it->second - rank_end) {
      filter_zero_ranges.push_back(std::make_pair(it->first - rank_start,
                                                  it->second - rank_end));
    }

    if (rank_start < rank_end) {
      filter_one_ranges.push_back(std::make_pair(rank_start + zero_count,
                                                 rank_end + zero_count));
    }
  }

  // 至少命中两个以上的label
  // bool has_zeros = (filter_zero_ranges.size() > 1);
  // bool has_ones = (filter_one_ranges.size() > 1);

  // 至少命中一个以上的label
  bool has_zeros = !filter_zero_ranges.empty();
  bool has_ones = !filter_one_ranges.empty();

  double zero_thres = thres, one_thres = thres;

  for (std::vector<std::pair<std::pair<size_t, size_t>, double> >
       ::const_iterator it = patterns.begin(); it != patterns.end(); ++it) {
    size_t rank_start = ba.Rank(1, (it -> first).first);
    size_t rank_end = ba.Rank(1, (it -> first).second);

    if (has_zeros) {
      if ((it->first).first - rank_start < (it->first).second - rank_end) {
        zero_ranges.push_back(std::make_pair(std::make_pair((it->first).first
                                                            - rank_start, (it->first).second - rank_end), it->second));
      } else {
        zero_thres -= it -> second;
        if (zero_thres < 0) {
          if (!has_ones) return;
          has_zeros = false;
        }
      }
    }

    if (has_ones) {
      if (rank_start < rank_end) {
        one_ranges.push_back(std::make_pair(std::make_pair(rank_start +
                                                           zero_count, rank_end + zero_count), it -> second));
      } else {
        one_thres -= it -> second;
        if (one_thres < 0) {
          if (!has_zeros) return;
          has_ones = false;
        }
      }
    }
  }
  if (has_zeros) {
    RetrievalWithFilterEachLevel(zero_ranges, filter_zero_ranges,
                                 zero_thres, level + 1, symbol, results);
  }
  if (has_ones) {
    RetrievalWithFilterEachLevel(one_ranges, filter_one_ranges, one_thres,
                                 level + 1, symbol | (int64_t)1 << (this->alphabet_bit_num_ - level - 1),
                                 results);
  }
}


void WaveletTree::RetrievalEachLevel(
  const std::vector<std::pair<std::pair<size_t, size_t>, double> >& patterns,
  double thres,
  size_t level,
  int64_t symbol,
  std::vector<int64_t>* results) {
  if (level == this->alphabet_bit_num_) {
    results -> push_back(symbol);
    return;
  }

  std::vector<std::pair<std::pair<size_t, size_t>, double> > zero_ranges,
      one_ranges;
  zero_ranges.reserve(patterns.size());
  one_ranges.reserve(patterns.size());

  double zero_thres = thres, one_thres = thres;
  bool has_zeros = true, has_ones = true;

  const wat_array::BitArray& ba = bit_arrays_[level];

  size_t zero_count = zero_counts_[level];

  for (std::vector<std::pair<std::pair<size_t, size_t>, double> >
       ::const_iterator it = patterns.begin(); it != patterns.end(); ++it) {
    size_t rank_start = ba.Rank(1, (it -> first).first);
    size_t rank_end = ba.Rank(1, (it -> first).second);

    if (has_zeros) {
      if ((it->first).first - rank_start < (it->first).second - rank_end) {
        zero_ranges.push_back(std::make_pair(std::make_pair((it->first).first -
                                                            rank_start, (it->first).second - rank_end), it->second));
      } else {
        zero_thres -= it -> second;
        if (zero_thres < 0) {
          if (!has_ones) return;
          has_zeros = false;
        }
      }
    }

    if (has_ones) {
      if (rank_start < rank_end) {
        one_ranges.push_back(std::make_pair(std::make_pair(rank_start +
                                                           zero_count, rank_end + zero_count), it->second));
      } else {
        one_thres -= it -> second;
        if (one_thres < 0) {
          if (!has_zeros) return;
          has_ones = false;
        }
      }
    }
  }
  if (has_zeros) {
    RetrievalEachLevel(zero_ranges, zero_thres, level + 1, symbol, results);
  }
  if (has_ones) {
    RetrievalEachLevel(one_ranges, one_thres, level + 1,
                       symbol | (int64_t)1 << (this->alphabet_bit_num_ - level - 1), results);
  }
}

void WaveletTree::IntersectEachLevel(
  const std::vector<std::pair<size_t, size_t> >& patterns,
  size_t thres,
  size_t max_count,
  size_t level,
  int64_t symbol,
  std::vector<int64_t>* results) {
  if ((*results).size() >= max_count) return;
  if (level == this->alphabet_bit_num_) {
    results -> push_back(symbol);
    return;
  }
  std::vector<std::pair<size_t, size_t> > zero_ranges, one_ranges;
  zero_ranges.reserve(patterns.size());
  one_ranges.reserve(patterns.size());

  size_t zero_thres = thres, one_thres = thres;
  bool has_zeros = true, has_ones = true;

  const wat_array::BitArray& ba = bit_arrays_[level];

  size_t zero_count = zero_counts_[level];

  for (std::vector<std::pair<size_t, size_t> >::const_iterator
       it = patterns.begin(); it != patterns.end(); ++it) {
    size_t rank_start = ba.Rank(1, it->first);
    size_t rank_end = ba.Rank(1, it->second);

    if (has_zeros) {
      if (it->first - rank_start < it->second - rank_end) {
        zero_ranges.push_back(std::make_pair(it->first - rank_start,
                                             it->second - rank_end));
      } else if (zero_thres-- == 0) {
        if (!has_ones) return;

        has_zeros = false;
      }
    }

    if (has_ones) {
      if (rank_start < rank_end) {
        one_ranges.push_back(std::make_pair(rank_start + zero_count,
                                            rank_end + zero_count));
      } else if (one_thres-- == 0) {
        if (!has_zeros) return;
        has_ones = false;
      }
    }
  }
  if (has_zeros) {
    IntersectEachLevel(zero_ranges, zero_thres, max_count, level + 1,
                       symbol, results);
  }
  if (has_ones) {
    IntersectEachLevel(one_ranges, one_thres, max_count, level + 1,
                       symbol | (int64_t)1 << (this->alphabet_bit_num_ - level - 1), results);
  }
}

}  // namespace dynamic_creative
}  // namespace gdt
