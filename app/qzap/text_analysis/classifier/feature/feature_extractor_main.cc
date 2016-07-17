// Copyright (c) 2012 Tencent Inc.
// Author: Pan Yang (baileyyang@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)
// Description: Extarct classifier features based on new FeatureExtractor

#include "app/qzap/text_analysis/classifier/feature/feature_extractor.h"

#include <fstream>  /// NOLINT

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/classifier/instance.h"
#include "app/qzap/text_analysis/text_miner_resource.h"
#include "app/qzap/text_analysis/text_miner.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

DEFINE_string(feature_vocabulary_file, "dict.feature_vocabulary",
              "the binary feature vocabulary file");

DECLARE_string(segmenter_data_dir);
DECLARE_string(text_miner_resource_config_file);
DECLARE_bool(extract_keytokens_only);

DEFINE_string(train_data_file, "train_data",
              "the labeled training data for classifier");
DEFINE_string(train_feature_file, "train_feature",
              "the outpu filet of training data' features");

DECLARE_bool(on_token_feature);
DECLARE_bool(on_keyword_feature);
DECLARE_bool(on_lda_feature);
DECLARE_bool(on_embedding_feature);

namespace qzap {
namespace text_analysis {

bool ExtractFeature(const std::vector<std::string>& label_comment,
                    const FeatureExtractor& feature_extractor,
                    TextMiner* text_miner,
                    std::string* output) {
  std::string label = label_comment[0];
  std::string comment = label_comment[1];
  Document document;
  Field* field = document.add_field();
  field->set_text(comment);
  field->set_weight(1.0);

  qzap::text_analysis::Instance instance;

  // extract token features
  if (FLAGS_on_token_feature) {
    if (!text_miner->ExtractTokens(&document)) {
      LOG(WARNING) << "Extract tokens failed";
    } else {
      feature_extractor.ExtractTokenFeature(document, &instance);
    }
  }

  // extract keyword features
  if (FLAGS_on_keyword_feature) {
    if (!text_miner->ExtractKeywords(&document)) {
      LOG(WARNING) << "Extract keywords failed";
    } else {
      feature_extractor.ExtractKeywordFeature(document, &instance);
    }
  }

  // extract lda features
  if (FLAGS_on_lda_feature) {
    if (!text_miner->InferTopics(&document)) {
      LOG(WARNING) << "Infer topics failed";
    } else {
      feature_extractor.ExtractTopicFeature(document, &instance);
    }
  }

  // NOTE: 此处不需要对Embedding特征做 L1-normalize？
  instance.L1Normalize();

  // extract embedding features
  if (FLAGS_on_embedding_feature) {
    if (!text_miner->InferEmbedding(&document)) {
      LOG(WARNING) << "Infer embedding failed";
    } else {
      feature_extractor.ExtractEmbeddingFeature(document, &instance);
    }
  }

  if (instance.Empty()) {
    return false;
  }

  std::string instance_str;
  instance.SerializeTo(&instance_str);
  StringAppendF(output,
                "%s\t%s\t%s\n",
                label.c_str(),
                instance_str.c_str(),
                comment.c_str());
  return true;
}

}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);

  std::ifstream fin(FLAGS_train_data_file.c_str());
  if (fin.fail()) {
    LOG(ERROR) << "train data file dose not exist";
    return -1;
  }

  std::ofstream fout(FLAGS_train_feature_file.c_str());
  if (fout.fail()) {
    LOG(ERROR) << "train feature file dose not exist";
    return -1;
  }

  scoped_ptr<qzap::text_analysis::TextMinerResource> text_miner_resource;
  text_miner_resource.reset(
      new qzap::text_analysis::TextMinerResource());
  CHECK(text_miner_resource->InitFromConfigFile(
          FLAGS_text_miner_resource_config_file));
  scoped_ptr<qzap::text_analysis::TextMiner> text_miner(
      new qzap::text_analysis::TextMiner(text_miner_resource.get()));

  qzap::text_analysis::FeatureExtractor feature_extractor;
  feature_extractor.LoadFeatureVocab(FLAGS_feature_vocabulary_file);
  std::string line;
  size_t count = 0;
  while (std::getline(fin, line) > 0) {
    TrimString(&line);
    std::vector<std::string> label_comment;
    SplitString(line, "\t", &label_comment);
    if (label_comment.size() != 2) {
      continue;
    }
    std::string output;
    if (!ExtractFeature(label_comment,
                       feature_extractor,
                       text_miner.get(),
                       &output)) {
      continue;
    }
    fout.write(output.data(), output.length());
    ++count;
    if (count % 1000 == 0) {
      LOG(INFO) << "Number of samples: " << count;
    }
  }

  fin.close();
  fout.close();
  LOG(INFO) << "Number of samples: " << count;

  return 0;
}
