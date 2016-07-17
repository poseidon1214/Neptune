// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#ifndef CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_SIMILARITY_RETRIEVALER_H_
#define CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_SIMILARITY_RETRIEVALER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include "data_collector/feeder/candidate/similarity/wavelet_tree.h"

namespace gdt {
namespace dynamic_creative {

typedef std::pair<uint64_t, double> IdWeightPair;
typedef std::vector<IdWeightPair> WeightedVecotr;
typedef std::vector<std::pair<int64_t, double> > SimilarDocments;
typedef std::map<uint64_t, WeightedVecotr> InvertIndex;
typedef std::map<uint64_t, WeightedVecotr> ForwardIndex;
typedef std::map<uint64_t, std::pair<uint64_t, uint64_t> > OffsetMap;
typedef std::map<uint64_t, std::vector<uint64_t> > LabelInvertIndex;
typedef std::map<uint64_t, std::vector<uint64_t> > LabelForwardIndex;

struct Document {
  uint64_t id;
  WeightedVecotr contents;
  std::vector<uint64_t> labels;
};

// 类余弦检索
class SimilarityRetrievaler {
 public:
  void Build(const std::string& filename);
  void BuildWithLabels(const std::string& filename);
  void DocmentsToTopKList(const std::string& input,
                          const std::string& output,
                          double filter_thres = 0.1);
  void DocmentsToTopKListWithLabels(const std::string& input,
                                    const std::string& output,
                                    double filter_thres = 0.1);
  void Retrieval(const WeightedVecotr& document, double thres,
                 std::vector<int64_t>* results);
  void RetrievalWithReranking(WeightedVecotr* document, double thres,
                              std::vector<std::pair<int64_t, double> >* filter_results,
                              int64_t* retrieval_time  = NULL,
                              int64_t* reanking_time = NULL);
  void RetrievalWithReranking(
    const WeightedVecotr& document,
    double thres, int64_t docid,
    std::vector<std::pair<int64_t, double> >* filter_results);
  void RetrievalWithLabelFilter(const WeightedVecotr& document,
                                const std::vector<uint64_t>& labels,
                                double thres,
                                std::vector<int64_t>* results);

  void Intersect(const vector<uint64_t>& termids, size_t thres,
                 std::vector<int64_t>* results);

  void RetrievalWithInputFile(
    const std::string& input,
    const std::string& output);

  void SplitDocuments(
    const std::vector<Document>& docs,
    uint32_t split_num,
    std::vector<std::vector<Document> >* docs_shards);
  void MultiThreadRetrieval(
    const std::vector<Document>& docs,
    std::vector<std::pair<uint64_t, SimilarDocments> >* results);
  void SingleThreadRetrieval(
    const std::vector<Document>& docs,
    std::vector<std::pair<uint64_t, SimilarDocments> >* results);
  void RetrievalWithLabelFilterAndReranking(
    const Document& doc,
    SimilarDocments* result);

 private:
  void ForwardIndexToInvertedIndex(const ForwardIndex& forward_index,
                                   InvertIndex* invert_index);
  void BuildWavletTree(const InvertIndex& invert_index);
  void BuildWavletTreeWithLabels(const InvertIndex& invert_index,
                                 const LabelInvertIndex label_invert_index);
  double CosineSimilarity(std::vector<IdWeightPair>* left,
                          std::vector<IdWeightPair>* right);

 private:
  WaveletTree wavelet_tree;
  OffsetMap feature_offset;
  OffsetMap label_offset;
  ForwardIndex forward_index;
  LabelForwardIndex label_forward_index;
};

}  // namespace dynamic_creative
}  // namespace gdt

#endif  // CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_SIMILARITY_RETRIEVALER_H_
