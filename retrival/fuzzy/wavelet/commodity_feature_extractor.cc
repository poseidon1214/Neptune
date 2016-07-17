// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include "creative/dynamic_creative/candidate/similarity/commodity_feature_extractor.h"
#include <utility>
#include <string>
#include <vector>
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "creative/textminer/text_miner_client.h"
#include "creative/content_analysis/service/content_analysis_client.h"
#include "creative/dynamic_creative/proto/commodity.pb.h"
#include "app/qzap/text_analysis/document_utils.h"

using gdt::creative::TextMinerRequest;
using gdt::creative::TextMinerResponse;
using qzap::text_analysis::DocumentUtils;

typedef uint64_t FeatureIDType;
typedef std::vector<std::pair<FeatureIDType, double> > CommodityFeature;

DEFINE_string(text_miner_config_file, "../conf/text_miner_resource.config",
              "text_miner_config_file");
DEFINE_bool(local_text_miner, true, "true,本地加载textminer; "
            "false:请求textminer服务");
DEFINE_double(commodity_name_weight, 1.0, "商品名称权重");
DEFINE_double(commodity_title_weight, 1.0, "描述特征权重");
DEFINE_int32(commodity_feature_num, 30, "最大提取特征数");

namespace gdt {
namespace dynamic_creative {

scoped_ptr<qzap::text_analysis::TextMinerResource>
    CommodityFeatureExtractor::text_miner_resource_;
scoped_ptr<qzap::text_analysis::TextMiner>
    CommodityFeatureExtractor::text_miner_;
gdt::creative::TextAnalysis CommodityFeatureExtractor::text_analysis;

void CommodityFeatureExtractor::Init() {
  if (FLAGS_local_text_miner) {
    LOG(INFO) << "Starting to load textminer model";
    text_miner_resource_.reset(new qzap::text_analysis::TextMinerResource());
    if (!text_miner_resource_->
        InitFromConfigFile(FLAGS_text_miner_config_file)) {
      LOG(ERROR) << "TextMinerResource InitFromConfigFile failed";
      return;
    }
    LOG(INFO) << "Initialize textminer succeeded";
  } else {
    text_analysis.Init();
  }
  if (text_miner_resource_) {
    text_miner_.reset(
      new qzap::text_analysis::TextMiner(text_miner_resource_.get()));
  }
}

bool CommodityFeatureExtractor::ExatrctFeature(
  const Commodity& commodity,
  CommodityFeature* commodity_feature) {
  TextMinerRequest text_request;
  qzap::text_analysis::Document* document = text_request.mutable_document();
  if (commodity.has_commodity_name()) {
    qzap::text_analysis::Field *field = document->add_field();
    field->set_type(qzap::text_analysis::UNKNOWN_FIELD);
    field->set_weight(FLAGS_commodity_name_weight);
    field->set_text(commodity.commodity_name());
  }
  if (commodity.has_commodity_description()) {
    qzap::text_analysis::Field *field = document->add_field();
    field->set_type(qzap::text_analysis::UNKNOWN_FIELD);
    field->set_weight(FLAGS_commodity_title_weight);
    field->set_text(commodity.commodity_description());
  }

  if (FLAGS_local_text_miner && text_miner_.get() && text_miner_resource_) {
    if (text_miner_->ExtractTokens(document)) {
      DocumentUtils::TruncateRepeatedField(
        FLAGS_commodity_feature_num, 0.01,
        document->mutable_bow_token());
      for (int i = 0; i < document->bow_token_size(); ++i) {
        commodity_feature->push_back(
          std::make_pair(document->bow_token(i).signature(),
                         document->bow_token(i).weight()));
      }
    } else {
      LOG(ERROR) << "infer topic error, text:" << document->field(0).text();
    }
    return true;
  }

  if (document->field_size() > 0) {
    // 利用server来请求
    text_request.set_option(FLAGS_request_option);
    text_request.set_is_trunc(FLAGS_is_trunc);
    text_request.set_is_revise_token_weight(FLAGS_is_revise_token_weight);
    TextMinerResponse response;
    if (text_analysis.DoTextAnalysis(&text_request, &response)) {
      DocumentUtils::TruncateRepeatedField(
        FLAGS_commodity_feature_num,
        0.01,
        response.mutable_document()->mutable_bow_token());
      for (int i = 0; i < response.document().bow_token_size(); ++i) {
        commodity_feature->push_back(
          std::make_pair(response.document().bow_token(i).signature(),
                         response.document().bow_token(i).weight()));
      }
    } else {
      return false;
    }
  } else {
    return false;
  }
  return true;
}

}  // namespace dynamic_creative
}  // namespace gdt
