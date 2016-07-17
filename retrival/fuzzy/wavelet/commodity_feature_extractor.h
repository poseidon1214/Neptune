// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#ifndef CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_COMMODITY_FEATURE_EXTRACTOR_H_
#define CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_COMMODITY_FEATURE_EXTRACTOR_H_

#include <utility>
#include <string>
#include <vector>
#include "app/qzap/common/base/scoped_ptr.h"
#include "thirdparty/glog/logging.h"
#include "app/qzap/text_analysis/text_miner.h"
#include "app/qzap/text_analysis/text_miner_resource.h"
#include "creative/content_analysis/service/content_analysis_client.h"
#include "creative/dynamic_creative/proto/commodity.pb.h"
#include "creative/textminer/text_miner_client.h"

typedef uint64_t FeatureIDType;
typedef std::vector<std::pair<FeatureIDType, double> > CommodityFeature;

namespace gdt {
namespace dynamic_creative {

class CommodityFeatureExtractor {
 public:
  static void Init();
  static bool ExatrctFeature(const Commodity& commodity,
                             CommodityFeature* commodity_feature);
 private:
  static scoped_ptr<qzap::text_analysis::TextMinerResource>
      text_miner_resource_;
  static scoped_ptr<qzap::text_analysis::TextMiner> text_miner_;

  static gdt::creative::TextAnalysis text_analysis;
};

}  // namespace dynamic_creative
}  // namespace gdt

#endif  // CREATIVE_DYNAMIC_CREATIVE_CANDIDATE_SIMILARITY_COMMODITY_FEATURE_EXTRACTOR_H_
