// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         YiXiang Mu (javiermu@tencent.com)
//         Pan Yang (baileyyang@tenceng.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/classifier/classifier.h"

#include <math.h>

#include "thirdparty/glog/logging.h"
#include "app/qzap/text_analysis/classifier/feature/feature_extractor.h"
#include "app/qzap/text_analysis/classifier/hierarchical_classifier.h"
#include "app/qzap/text_analysis/classifier/instance.h"
#include "app/qzap/text_analysis/classifier/taxonomy_hierarchy.h"
#include "app/qzap/text_analysis/dict/vocabulary.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

DEFINE_double(classifier_threshold, 0.2,
              "the threshold of classifier predict result");


DEFINE_bool(on_token_feature, true, "whether or not to use token feature");
DEFINE_bool(on_keyword_feature, false,
            "whether or not to use keyword feature");
DEFINE_bool(on_lda_feature, true, "whether or not to use lda feture");
DEFINE_bool(on_embedding_feature, false,
            "whether or not to use embedding feture");

namespace qzap {
namespace text_analysis {

Classifier::Classifier() {}

Classifier::~Classifier() {}

bool Classifier::LoadModel(const std::string& model_dir) {
  classifier_.reset(new HierarchicalClassifier);
  CHECK(classifier_->LoadFromDir(model_dir)) << "Load classfier model failed";
  feature_extractor_.reset(new FeatureExtractor);
  CHECK(feature_extractor_->LoadFeatureVocab(model_dir + "/feature.vocabulary"))
      << "Load feature vocabulary failed";

  return true;
}

bool Classifier::Predict(Document* document) const {
  Instance instance;
  HierarchicalClassifier::Result result;

  ExtractFeatures(*document, &instance);
  if (instance.Empty()) { return true; }

  classifier_->Predict(instance, &result);

  uint32_t taxonomy_depth =
      static_cast<uint32_t>(classifier_->taxonomy().Depth());
  if (result.size() == taxonomy_depth) {
    double sum = 0.0;
    for (size_t i = 0; i < result[taxonomy_depth - 1].size(); ++i) {
      if (result[taxonomy_depth - 1][i].probability() <
          FLAGS_classifier_threshold) {
        break;
      }
      Category* category = document->add_category();
      category->set_name(
          classifier_->taxonomy().Name(result[taxonomy_depth - 1][i].id()));
      category->set_id(result[taxonomy_depth - 1][i].id());
      category->set_weight(result[taxonomy_depth - 1][i].probability());
      category->set_ori_weight(category->weight());
      sum += category->weight() * category->weight();
    }
    sum = sqrt(sum);

    double kEpsilon = 1E-6;
    if (sum > kEpsilon) {
      for (int i = 0; i < document->category_size(); ++i) {
        document->mutable_category(i)->set_weight(
            document->category(i).weight() / sum);
      }
    }
  } else {
    VLOG(30) << "No " << taxonomy_depth << " level classifier result";
  }

  return true;
}

void Classifier::ExtractFeatures(const Document& document,
                                 Instance* instance) const {
  // Extract token features
  if (FLAGS_on_token_feature) {
    if (!document.has_extracted_token()) {
      VLOG(30) << "[classifier]: token feature not ready";
    } else {
      feature_extractor_->ExtractTokenFeature(document, instance);
    }
  }

  // Extract keyword features
  if (FLAGS_on_keyword_feature) {
    if (!document.has_extracted_keyword()) {
      VLOG(30) << "[classifier]: keywords feature not ready";
    } else {
      feature_extractor_->ExtractKeywordFeature(document, instance);
    }
  }

  // Extract lda features
  if (FLAGS_on_lda_feature) {
    if (!document.has_infered_topic()) {
      VLOG(30) << "[classifier]: lda feature not ready";
    } else {
      feature_extractor_->ExtractTopicFeature(document, instance);
    }
  }

  // Extract embedding features
  if (FLAGS_on_lda_feature) {
    if (!document.has_infered_embedding()) {
      VLOG(30) << "[classifier]: embedding feature not ready";
    } else {
      feature_extractor_->ExtractEmbeddingFeature(document, instance);
    }
  }

  // NOTE(fandywang): 是否需要做 L1-Normalize？
  instance->L1Normalize();
}

}  // namespace text_analysis
}  // namespace qzap

