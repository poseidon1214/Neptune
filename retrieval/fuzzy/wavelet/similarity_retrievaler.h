// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#ifndef CREATIVE_wavelet_CANDIDATE_SIMILARITY_SIMILARITY_RETRIEVALER_H_
#define CREATIVE_wavelet_CANDIDATE_SIMILARITY_SIMILARITY_RETRIEVALER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <tuple>
#include "retrieval/fuzzy/wavelet/wavelet_tree.h"
#include "retrieval/proto/document.pb.h"
#include "retrieval/proto/query.pb.h"

namespace gdt {
namespace wavelet {

typedef std::map<uint64_t, std::pair<uint64_t, uint64_t> > FieldOffsetMap;

struct FieldOffset {
  uint64_t begin;
  uint64_t end;
  FieldOffsetMap field_offset_map;
};

// TODO(cernwang) 这边设计的有问题， 都用typedef后续有新需求变化会有很大改动
typedef std::map<uint64_t, FieldOffset> OffsetMap;
typedef std::vector<std::tuple<size_t, size_t, double> > FieldPattern;
typedef std::vector<std::pair<FieldPattern, double> > QueryPattern;
typedef std::map<uint64_t, std::vector<uint64_t> > FieldInvertIndex;
typedef std::map<uint64_t, FieldInvertIndex> InvertIndex;

// 类余弦检索
class SimilarityRetrievaler {
 public:
  bool Build(const std::vector<Document>& documents);
  bool Retrieval(const Query& query, std::vector<Result>* results);

 private:
  bool BuildDocument(const Document& document);

  bool BuildFiled(const Field& field, uint64_t docid);

  bool BuildIdFiled(const Field& field, uint64_t docid);
  
  bool BuildNumFiled(const Field& field, uint64_t docid);

  bool BuildFieldPattern(
      const FieldOffset& field_offset,
      const FieldQuery& field_query,
      std::pair<FieldPattern, double>* field_pattern_thres);  

  bool BuildIdFieldPattern(
      const FieldOffset& field_offset,
      const IdQuery& id_query,
      FieldPattern* patterns);

  bool BuildRangeFieldPattern(
      const FieldOffset& field_offset,
      const RangeQuery& range_query,
      FieldPattern* patterns);

  bool BuildWavletTree(const InvertIndex& invert_index);

  bool BuildFieldOffset(
      const FieldInvertIndex& field_invert_index,
      FieldOffsetMap& field_offset_map,
      std::vector<uint64_t>& posting);

 private:
  WaveletTree wavelet_tree;
  OffsetMap feature_offset;
  InvertIndex invert_index;
};

}  // namespace wavelet
}  // namespace gdt

#endif  // CREATIVE_wavelet_CANDIDATE_SIMILARITY_SIMILARITY_RETRIEVALER_H_
