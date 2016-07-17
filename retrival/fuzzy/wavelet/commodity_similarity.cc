// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include "creative/dynamic_creative/candidate/similarity/commodity_similarity.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"
#include "app/qzap/common/base/walltime.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/time_utility.h"
#include "app/qzap/service/ad_portrait/creek/wordfield/core/wordfield_commonfunc.h"
#include "creative/dynamic_creative/candidate/similarity/similarity_retrievaler.h"
#include "creative/dynamic_creative/common/material_common.h"
#include "creative/dynamic_creative/common/material_target_id_alloc.h"

DEFINE_int32(top_k_num, 20, "最大相似商品数");
DEFINE_double(thres, 0.4, "similarity thres");
DEFINE_bool(use_orgin_function, false, "use_orgin_function");

namespace gdt {
namespace dynamic_creative {

typedef std::vector<std::pair<uint64_t, double> > PostingList;

class SimilarityCmp {
 public:
  bool operator()(const std::pair<uint64_t, double>& left,
                  const std::pair<uint64_t, double>& right) const {
    return left.second > right.second;
  }
};

static std::vector<std::string> Split(const std::string& src,
                                      const std::string& separator) {
  std::vector<std::string> dest;
  std::string str = src;
  std::string substring;
  std::string::size_type start = 0, index;
  do {
    index = str.find_first_of(separator, start);
    if (index != std::string::npos) {
      substring = str.substr(start, index - start);
      dest.push_back(substring);
      start = str.find_first_not_of(separator, index);
      if (start == std::string::npos) return dest;
    }
  } while (index != std::string::npos);
  substring = str.substr(start);
  dest.push_back(substring);
  return dest;
}

void CommoditySimilarity::WriteDocmentFeatures(
    const std::string& filename,
    const std::vector<Commodity>& commodities) {
  WriteDocmentFeaturesWithLocalId(filename, commodities);
}


void CommoditySimilarity::WriteDocmentFeaturesWithLocalId(
    const std::string& filename,
    const std::vector<Commodity>& commodities) {
  std::ofstream ofs(filename.c_str());
  for (uint64_t i = 0; i < commodities.size(); i++) {
    CommodityFeature commodity_feature;
    CommodityFeatureExtractor::ExatrctFeature(commodities[i],
                                              &commodity_feature);
    ofs << i << "<";
    for (int j = 0; j < commodities[i].labels_size(); j++) {
      ofs << OfflineCommon::ToNum(commodities[i].labels(j));
      if (j + 1 < commodities[i].labels_size()) {
        ofs << ",";
      }
    }
    ofs << ">";
    for (uint64_t j = 0; j < commodity_feature.size(); j++) {
      ofs << "\t" << commodity_feature[j].first << ":"
          << commodity_feature[j].second;
    }
    ofs << std::endl;
  }
  ofs.close();
}

void CommoditySimilarity::WriteDocmentFeaturesWithCommodityId(
    const std::string& filename,
    const std::vector<Commodity>& commodities) {
  std::ofstream ofs(filename.c_str());
  for (uint64_t i = 0; i < commodities.size(); i++) {
    CommodityFeature commodity_feature;
    CommodityFeatureExtractor::ExatrctFeature(commodities[i],
                                              &commodity_feature);
    ofs << commodities[i].commodity_id() << "<";
    for (int j = 0; j < commodities[i].labels_size(); j++) {
      ofs << OfflineCommon::ToNum(commodities[i].labels(j));
      if (j + 1 < commodities[i].labels_size()) {
        ofs << ",";
      }
    }
    ofs << ">";
    for (uint64_t j = 0; j < commodity_feature.size(); j++) {
      ofs << "\t" << commodity_feature[j].first << ":"
          << commodity_feature[j].second;
    }
    ofs << std::endl;
  }
  ofs.close();
}

void CommoditySimilarity::ComputeSimilarities(
    const std::string& index_feature_file,
    const std::string& similar_list_file,
    const std::string& whole_feature_file) {
  SimilarityRetrievaler similarity_retrievaler;
  if (FLAGS_use_orgin_function) {
    similarity_retrievaler.DocmentsToTopKListWithLabels(index_feature_file,
                                                        similar_list_file,
                                                        FLAGS_thres);
  } else {
    int64_t begin_time = GetCurrentTimeMillis();
    similarity_retrievaler.BuildWithLabels(index_feature_file);
    int64_t mid_time = GetCurrentTimeMillis();
    LOG(INFO) << "Build time:" << mid_time - begin_time << "ms";
    similarity_retrievaler.RetrievalWithInputFile(whole_feature_file,
                                                  similar_list_file);
    int64_t end_time = GetCurrentTimeMillis();
    LOG(INFO) << "Retrieval time:" << end_time - mid_time << "ms";
  }
}

void CommoditySimilarity::InvertFileToForwardFile(
    const std::string input, const std::string output) {
  ForwardIndex forward_index;
  std::string line;
  std::ifstream ifs(input.c_str());
  std::ofstream ofs(output.c_str());
  while (getline(ifs, line)) {
    vector<string> raw_doc = Split(line, "\t");
    if (raw_doc.size() < 1) {
      std::cout << "File Format Error" << std::endl;
    } else {
      uint64_t docid;
      CHECK(StringToNumeric(raw_doc[0], &docid));
      for (uint64_t i = 1; i < raw_doc.size(); i++) {
        vector<string> raw_pair = Split(raw_doc[i], ":");
        uint64_t similar_id;
        CHECK(StringToNumeric(raw_pair[0], &similar_id));
        double similarity;
        CHECK(StringToNumeric(raw_pair[1], &similarity));
        forward_index[similar_id].push_back(std::make_pair(docid, similarity));
      }
    }
  }
  for (ForwardIndex::const_iterator it = forward_index.begin();
       it != forward_index.end(); it++) {
    ofs << it -> first;
    for (size_t i = 0; i < (it -> second).size(); i++) {
      ofs << "\t" << (it -> second)[i].first << ":"
          << (it -> second)[i].second;
    }
    ofs << std::endl;
  }
  ifs.close();
  ofs.close();
}

void CommoditySimilarity::ReadSimilarLists(
    const std::string& filename,
  std::vector<Commodity>* commodities) {
  ReadSimilarLocalIdLists(filename, commodities);
}

void CommoditySimilarity::ReadSimilarLocalIdLists(
    const std::string& filename,
  std::vector<Commodity>* commodities) {
  std::ifstream ifs(filename.c_str());
  std::string line;
  getline(ifs, line);
  while (!ifs.eof()) {
    std::vector<std::string> raw_doc = Split(line, "\t");
    if (raw_doc.size() < 1) {
      LOG(ERROR) << "Stream Format Error:" << line;
    } else {
      uint64_t key_id;
      CHECK(StringToNumeric(raw_doc[0], &key_id));
      MaterialTargeting& target_id_list =
        (*(*commodities)[key_id].mutable_target_id_list());
      for (uint64_t i = 1; i < raw_doc.size(); i++) {
        std::vector<std::string> raw_pair = Split(raw_doc[i], ":");
        uint64_t value_id;
        CHECK(StringToNumeric(raw_pair[0], &value_id));
        double similarity;
        CHECK(StringToNumeric(raw_pair[1], &similarity));
        MaterialTargeting::Targeting* targeting_term =
          target_id_list.add_targeting();
        (*targeting_term).set_id(material_target::GenerateSimilarTargetingID(
                                   (*commodities)[value_id].commodity_id()));
        (*targeting_term).set_type(
          MaterialTargeting_TargetingType_kSimilarMaterial);
        (*targeting_term).set_weight(similarity);
      }
    }
    getline(ifs, line);
  }
}

void CommoditySimilarity::ReadSimilarCommodityIdLists(
    const std::string& filename,
    std::vector<Commodity>* commodities) {
  std::ifstream ifs(filename.c_str());
  std::string line;
  getline(ifs, line);
  while (!ifs.eof()) {
    std::vector<std::string> raw_doc = Split(line, "\t");
    if (raw_doc.size() < 1) {
      LOG(ERROR) << "Stream Format Error:" << line;
    } else {
      uint64_t key_id;
      CHECK(StringToNumeric(raw_doc[0], &key_id));
      MaterialTargeting& target_id_list =
        (*(*commodities)[key_id].mutable_target_id_list());
      // 最多写40个targeting id
      for (uint64_t i = 1; i < std::min(int(raw_doc.size()), 40); i++) {
        std::vector<std::string> raw_pair = Split(raw_doc[i], ":");
        uint64_t commodity_id;
        CHECK(StringToNumeric(raw_pair[0], &commodity_id));
        double similarity;
        CHECK(StringToNumeric(raw_pair[1], &similarity));
        MaterialTargeting::Targeting* targeting_term =
          target_id_list.add_targeting();
        (*targeting_term).set_id(material_target::GenerateSimilarTargetingID(
                                   commodity_id));
        (*targeting_term).set_type(
          MaterialTargeting_TargetingType_kSimilarMaterial);
        (*targeting_term).set_weight(similarity);
      }
    }
    getline(ifs, line);
  }
}

void CommoditySimilarity::BuildIndexMap() {
  std::string line;
  getline(std::cin, line);
  while (!std::cin.eof()) {
    std::vector<std::string> raw_doc = Split(line, "\t");
    if (raw_doc.size() < 1) {
      LOG(ERROR) << "Stream Format Error:" << line;
    } else {
      uint64_t doc_id;
      CHECK(StringToNumeric(raw_doc[0], &doc_id));

      for (uint64_t i = 1; i < raw_doc.size(); i++) {
        std::vector<std::string> raw_pair = Split(raw_doc[i], ":");
        uint64_t term_id;
        CHECK(StringToNumeric(raw_pair[0], &term_id));
        double weight;
        CHECK(StringToNumeric(raw_pair[1], &weight));
        std::cout << term_id << "\t" << doc_id << "\t" << weight << std::endl;
      }
    }
    getline(std::cin, line);
  }
}

void CommoditySimilarity::BuildIndexReduce() {
  std::string line;
  uint64_t last_term_id = uint64_t(-1);
  getline(std::cin, line);
  while (!std::cin.eof()) {
    std::vector<std::string> raw_tuple = Split(line, "\t");
    if (raw_tuple.size() != 3) {
      if (last_term_id != uint64_t(-1)) {
        LOG(ERROR) << "Stream Format Error:" << line;
      }
    } else {
      uint64_t term_id;
      CHECK(StringToNumeric(raw_tuple[0], &term_id));
      uint64_t doc_id;
      CHECK(StringToNumeric(raw_tuple[1], &doc_id));
      double weight;
      CHECK(StringToNumeric(raw_tuple[2], &weight));
      if (last_term_id != term_id) {
        if (last_term_id != uint64_t(-1)) {
          std::cout << std::endl;
        }
        std::cout << term_id;
      }
      std::cout << "\t"  << doc_id << ":" << weight;
      last_term_id = term_id;
    }
    getline(std::cin, line);
  }
  std::cout << std::endl;
}

void CommoditySimilarity::ComputeSimilarityMap() {
  std::string line;
  getline(std::cin, line);
  while (!std::cin.eof()) {
    std::vector<std::string> raw_doc = Split(line, "\t");
    if (raw_doc.size() < 1) {
      LOG(ERROR) << "Stream Format Error:" << line;
    } else {
      PostingList posting;
      for (uint64_t i = 1; i < raw_doc.size(); i++) {
        std::vector<std::string> raw_pair = Split(raw_doc[i], ":");
        uint64_t doc_id;
        CHECK(StringToNumeric(raw_pair[0], &doc_id));
        double weight;
        CHECK(StringToNumeric(raw_pair[1], &weight));
        posting.push_back(std::make_pair(doc_id, weight));
      }
      for (uint64_t i = 0; i < posting.size(); i++) {
        for (uint64_t j = 0; j < posting.size(); j++) {
          if (i == j) {
            continue;
          }
          std::cout << posting[i].first << "\t"
                    << posting[j].first << "\t"
                    << posting[i].second*posting[j].second
                    << std::endl;
        }
      }
    }
    getline(std::cin, line);
  }
}

void CommoditySimilarity::ComputeSimilarityReduce() {
  std::string line;
  uint64_t last_doc_id1 = uint64_t(-1);
  uint64_t last_doc_id2 = uint64_t(-1);
  double similarity = 0;
  std::vector<std::pair<uint64_t, double> > similarity_pair_list;
  SimilarityCmp similarity_cmp;
  getline(std::cin, line);
  while (!std::cin.eof()) {
    std::vector<std::string> raw_tuple = Split(line, "\t");
    if (raw_tuple.size() != 3) {
      if (last_doc_id1 != uint64_t(-1)) {
        LOG(ERROR) << "Stream Format Error:" << line;
      }
    } else {
      uint64_t doc_id1;
      CHECK(StringToNumeric(raw_tuple[0], &doc_id1));
      uint64_t doc_id2;
      CHECK(StringToNumeric(raw_tuple[1], &doc_id2));
      double weight_product;
      CHECK(StringToNumeric(raw_tuple[2], &weight_product));

      if (doc_id1 == last_doc_id1 && doc_id2 == last_doc_id2) {
        similarity += weight_product;
      } else {
        if (last_doc_id1 != uint64_t(-1) && last_doc_id2 != uint64_t(-1)) {
          similarity_pair_list.push_back(std::make_pair(last_doc_id2,
                                                        similarity));
          if (doc_id1 != last_doc_id1) {
            std::sort(similarity_pair_list.begin(),
                      similarity_pair_list.end(), similarity_cmp);
            std::cout << last_doc_id1;
            for (uint64_t i = 0; i < std::min(similarity_pair_list.size(),
                                              size_t(FLAGS_top_k_num)); i++) {
              std::cout << "\t" << similarity_pair_list[i].first << ":"
                        << similarity_pair_list[i].second;
            }
            std::cout << std::endl;
            similarity_pair_list.clear();
          }
        }
        similarity = weight_product;
      }
      last_doc_id1 = doc_id1;
      last_doc_id2 = doc_id2;
    }
    getline(std::cin, line);
  }
  similarity_pair_list.push_back(std::make_pair(last_doc_id2, similarity));
  std::sort(similarity_pair_list.begin(),
            similarity_pair_list.end(), similarity_cmp);
  std::cout << last_doc_id1;
  for (uint64_t i = 0; i < std::min(similarity_pair_list.size(),
                                    size_t(FLAGS_top_k_num)); i++) {
    std::cout << "\t" << similarity_pair_list[i].first << ":"
              << similarity_pair_list[i].second;
  }
  std::cout << std::endl;
  similarity_pair_list.clear();
}

}  // namespace dynamic_creative
}  // namespace gdt

