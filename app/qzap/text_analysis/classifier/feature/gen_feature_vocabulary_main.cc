// Copyright (c) 2012 Tencent Inc.
// Author: Pan Yang (baileyyang@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)
//
// Generate a feature vocabulary and use the dict vocabulary to manage
// the <feature_string, feature_id> table

#include <fstream>
#include <set>
#include <tr1/unordered_map>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/text_miner_resource.h"
#include "app/qzap/text_analysis/text_miner.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

DECLARE_string(text_miner_resource_config_file);
DECLARE_string(segmenter_data_dir);
DECLARE_int32(peacock_cache_size_mb);  // default = 5 * 1024
DECLARE_int32(peacock_num_markov_chains);  // default = 5
DECLARE_int32(peacock_total_iterations);  // default = 15
DECLARE_int32(peacock_burn_in_iterations);  // default = 10
DECLARE_int32(peacock_topic_top_k);
DECLARE_int32(peacock_topic_word_top_k);

DECLARE_bool(on_token_feature);
DECLARE_bool(on_keyword_feature);
DECLARE_bool(on_lda_feature);
DECLARE_bool(on_embedding_feature);

DEFINE_string(train_data_file, "train_data",
              "the labeled training data for classifier");
DEFINE_string(feature_vocabulary_file, "dict.feature_vocabulary",
              "the binary feature vocabulary file");

namespace qzap {
namespace text_analysis {

void ExtractFeature(const std::vector<std::string>& label_comment,
                    TextMiner* text_miner,
                    std::set<std::string>* features) {
  std::string label = label_comment[0];
  std::string comment = label_comment[1];
  Document document;
  Field* field = document.add_field();
  field->set_text(comment);
  field->set_weight(1.0);

  if (FLAGS_on_token_feature) {
    if (!text_miner->ExtractTokens(&document)) {
      LOG(WARNING) << "Extract tokens failed";
    } else {
      for (int i = 0; i < document.bow_token_size(); ++i) {
        if (document.bow_token(i).weight() > 0) {
          std::string token_feature;
          StringAppendF(&token_feature, "1-%s",
                        document.bow_token(i).text().c_str());
          features->insert(token_feature);
        }
      }
    }
  }

  if (FLAGS_on_keyword_feature) {
    if (!text_miner->ExtractKeywords(&document)) {
      LOG(WARNING) << "Extract keywords failed";
    } else {
      for (int i = 0; i < document.bow_keyword_size(); ++i) {
        std::string keyword_feature;
        StringAppendF(&keyword_feature, "2-%s",
                      document.bow_keyword(i).text().c_str());
        features->insert(keyword_feature);
      }
    }
  }

  if (FLAGS_on_lda_feature) {
    if (!text_miner->InferTopics(&document)) {
      LOG(WARNING) << "Infer topics failed";
    } else {
      for (int i = 0; i < document.topic_size(); ++i) {
        std::string topic_feature;
        StringAppendF(&topic_feature, "3-%d", document.topic(i).id());
        features->insert(topic_feature);
      }
    }
  }

  if (FLAGS_on_embedding_feature) {
    if (!text_miner->InferEmbedding(&document)) {
      LOG(WARNING) << "Infer embedding failed";
    } else {
      for (int i = 0; i < document.embedding_size(); ++i) {
        std::string embedding_feature;
        StringAppendF(&embedding_feature, "4-%d", i);
        features->insert(embedding_feature);
      }
    }
  }
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

  std::ofstream fout(FLAGS_feature_vocabulary_file.c_str());
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

  std::string line;
  std::set<std::string> features;
  size_t count = 0;
  while (std::getline(fin, line)) {
    TrimString(&line);
    std::vector<std::string> label_comment;
    SplitString(line, "\t", &label_comment);
    if (label_comment.size() != 2) {
      continue;
    }
    ExtractFeature(label_comment, text_miner.get(), &features);
    ++count;
    if (count % 1000 == 0) {
      LOG(INFO) << "Number of samples: " << count;
    }
  }

  for (std::set<std::string>::iterator iter = features.begin();
       iter != features.end(); ++iter) {
    fout.write((*iter).data(), (*iter).length());
    fout.write("\n", 1);
  }

  fin.close();
  fout.close();

  LOG(INFO) << "Number of samples: " << count;
  LOG(INFO) << "Number of features: " << features.size();
  return 0;
}
