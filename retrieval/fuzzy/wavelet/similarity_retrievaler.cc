// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include <math.h>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <sys/stat.h>
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/base/callback.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/sysinfo.h"
#include "app/qzap/common/base/walltime.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/thread/threadpool.h"
#include "app/qzap/common/thread/mutex.h"
#include "app/qzap/common/sstable/sstable.h"
#include "app/qzap/common/utility/file_utility.h"
#include "app/qzap/common/utility/time_utility.h"
#include "framework/common/base_functor.h"
#include "retrieval/fuzzy/wavelet/similarity_retrievaler.h"

namespace gdt {
namespace wavelet {

bool SimilarityRetrievaler::Build(const std::vector<Document>& documents) {
  std::for_each(documents.begin(), documents.end(),
                std::bind(&SimilarityRetrievaler::BuildDocument, this, std::placeholders::_1));
  return BuildWavletTree(invert_index);
}

bool SimilarityRetrievaler::BuildDocument(const Document& document) {
  std::for_each(document.field().begin(), document.field().end(),
                std::bind(&SimilarityRetrievaler::BuildFiled, this, std::placeholders::_1, document.index()));
  return true;
}

bool SimilarityRetrievaler::BuildFiled(const Field& field, uint64_t docid) {
  switch (field.field_type()) {
    case TYPE_NUM:
      return BuildNumFiled(field, docid);
    case TYPE_ID:
      return BuildIdFiled(field, docid);
    default:
      return false;
  } 
  return true;
}

bool SimilarityRetrievaler::BuildIdFiled(const Field& field, uint64_t docid) {
  std::for_each(field.id().begin(), field.id().end(), 
                [&] (uint64_t id) {
                  invert_index[field.field_id()][id].push_back(docid);
                });
  return true;
}

bool SimilarityRetrievaler::BuildNumFiled(const Field& field, uint64_t docid) {
  // TODO(cernwang) 间隔改成可配置的
  uint64_t id = uint64_t(field.num());
  invert_index[field.field_id()][id].push_back(docid);
  return true;
}

bool SimilarityRetrievaler::Retrieval(
    const Query& query, std::vector<Result>* results) {
  QueryPattern query_pattern;
  for (auto field_query : query.field_query()) {
    auto iter = feature_offset.find(field_query.field_id());
    CHECK(iter != feature_offset.end());
    std::pair<FieldPattern, double> field_pattern_thres;
    CHECK(BuildFieldPattern(iter->second, field_query, &field_pattern_thres));
    query_pattern.push_back(field_pattern_thres);
  }
  std::vector<uint64_t> doc_index;
  CHECK(wavelet_tree.Retrieval(query_pattern, &doc_index));
  return true;
}

bool SimilarityRetrievaler::BuildFieldPattern(
    const FieldOffset& field_offset,
    const FieldQuery& field_query,
    std::pair<FieldPattern, double>* field_pattern_thres) {
  switch (field_query.field_query_type()) {
    case FIELD_QUERY_ID:
      field_pattern_thres->second = field_query.id_query().thres();
      return BuildIdFieldPattern(field_offset, field_query.id_query(), &(field_pattern_thres->first));
    case FIELD_QUERY_RANGE:
      field_pattern_thres->second = 1;
      return BuildRangeFieldPattern(field_offset, field_query.range_query(), &(field_pattern_thres->first));
    default:
      break;
  }
  return true;
}

bool SimilarityRetrievaler::BuildIdFieldPattern(
    const FieldOffset& field_offset,
    const IdQuery& id_query,
    FieldPattern* field_pattern) {
  auto& field_offset_map = field_offset.field_offset_map;
  for (auto& token: id_query.token()) {
    auto iter = field_offset_map.find(token.id()); 
    CHECK_CONTINUE(iter != field_offset_map.end());
    field_pattern->push_back(std::make_tuple(iter->second.first, iter->second.second, token.weight()));
  }
  return true;
}

bool SimilarityRetrievaler::BuildRangeFieldPattern(
    const FieldOffset& field_offset,
    const RangeQuery& range_query,
    FieldPattern* field_pattern) {
  auto& field_offset_map = field_offset.field_offset_map;
  auto lower_iter = field_offset_map.lower_bound(range_query.lower_bound());
  auto upper_iter = field_offset_map.upper_bound(range_query.upper_bound());
  uint64_t begin = (lower_iter != field_offset_map.begin() && range_query.has_lower_bound()) ? lower_iter->second.first : field_offset.begin;
  uint64_t end = (upper_iter != field_offset_map.end() && range_query.has_upper_bound()) ? upper_iter->second.second : field_offset.end;
  field_pattern->push_back(std::make_tuple(begin, end, 1));
  return true;
}


bool SimilarityRetrievaler::BuildWavletTree(const InvertIndex& invert_index) {
  std::vector<uint64_t> posting;
  for (auto field_invert_index : invert_index) {
    FieldOffset field_offset;
    field_offset.begin = posting.size();
    BuildFieldOffset(field_invert_index.second, field_offset.field_offset_map, posting);
    field_offset.end = posting.size();
    feature_offset[field_invert_index.first] = std::move(field_offset);
  }
  wavelet_tree.Init(posting);
  return true;
}

bool SimilarityRetrievaler::BuildFieldOffset(
    const FieldInvertIndex& field_invert_index,
    FieldOffsetMap& field_offset_map,
    std::vector<uint64_t>& posting) {
  for (auto field_docs : field_invert_index) {
    field_offset_map[field_docs.first].first = posting.size();
    std::copy(field_docs.second.begin(), field_docs.second.end(), std::back_inserter(posting));
    field_offset_map[field_docs.first].second = posting.size();
  }
  return true;
}


}  // namespace wavelet
}  // namespace gdt
