// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#ifndef CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_COMMODITY_SIMILARITY_H_
#define CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_COMMODITY_SIMILARITY_H_

#include <utility>
#include <vector>
#include <string>
#include "creative/dynamic_creative/candidate/similarity/commodity_feature_extractor.h"

typedef std::vector<std::pair<uint64_t, double> > TopKSimlarCommodity;

namespace gdt {
namespace dynamic_creative {

class CommoditySimilarity {
 public:
  static void WriteDocmentFeatures(
    const std::string& filename,
    const std::vector<Commodity>& commodities);
  static void WriteDocmentFeaturesWithLocalId(
    const std::string& filename,
    const std::vector<Commodity>& commodities);
  static void WriteDocmentFeaturesWithCommodityId(
    const std::string& filename,
    const std::vector<Commodity>& commodities);
  static void InvertFileToForwardFile(
    const std::string input,
    const std::string output);
  static void ReadSimilarLists(
    const std::string& filename,
    std::vector<Commodity>* commodities);
  static void ReadSimilarLocalIdLists(
    const std::string& filename,
    std::vector<Commodity>* commodities);
  static void ReadSimilarCommodityIdLists(
    const std::string& filename,
    std::vector<Commodity>* commodities);
  static void ComputeSimilarities(
    const std::string& feature_file,
    const std::string& similar_list_file,
    const std::string& whole_feature_file);
  static void BuildIndexMap();
  static void BuildIndexReduce();
  static void ComputeSimilarityMap();
  static void ComputeSimilarityReduce();
};

}  // namespace dynamic_creative
}  // namespace gdt

#endif  // CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_COMMODITY_SIMILARITY_H_
