// Copyright (c) 2012 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/classifier/evaluation/classifier_evaluator.h"

#include <tr1/unordered_map>
#include <fstream>
#include <string>
#include <vector>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/classifier/classifier.pb.h"
#include "app/qzap/text_analysis/classifier/feature/feature_extractor.h"
#include "app/qzap/text_analysis/classifier/hierarchical_classifier.h"
#include "app/qzap/text_analysis/classifier/taxonomy_hierarchy.h"
#include "app/qzap/text_analysis/text_miner_resource.h"
#include "app/qzap/text_analysis/text_miner.h"

DEFINE_int32(topk_results, 1,
             "the topk predict results which will compared with "
             "standard category.");

DECLARE_bool(on_token_feature);
DECLARE_bool(on_keyword_feature);
DECLARE_bool(on_lda_feature);
DECLARE_bool(on_embedding_feature);

namespace qzap {
namespace text_analysis {

ClassifierEvaluator::ClassifierEvaluator() {
  text_miner_resource_.reset(new TextMinerResource());
  CHECK(text_miner_resource_->InitFromConfigFile(
          FLAGS_text_miner_resource_config_file));
  text_miner_.reset(new TextMiner(text_miner_resource_.get()));
}

ClassifierEvaluator::~ClassifierEvaluator() {}

bool ClassifierEvaluator::LoadClassifierModel(
    const std::string& classifier_model_dir,
    const std::string& feature_vocabulary_file) {
  hierarchical_classifier_.reset(new HierarchicalClassifier());
  CHECK(hierarchical_classifier_->LoadFromDir(classifier_model_dir));
  feature_extractor_.reset(new FeatureExtractor);
  CHECK(feature_extractor_->LoadFeatureVocab(feature_vocabulary_file));
  return true;
}

bool ClassifierEvaluator::LoadTestCorpus(const std::string& test_data_file) {
  std::ifstream fs(test_data_file.c_str());
  if (fs.fail()) {
    LOG(ERROR) << "test data file dose not exist";
    return false;
  }

  std::string line;
  while (std::getline(fs, line)) {
    TrimString(&line);
    size_t pos = line.find("\t");
    if (pos == std::string::npos) {
      LOG(WARNING) << "The label is null, line: " << line;
      continue;
    }
    if (pos + 1 == line.size()) {
      LOG(WARNING) << "The text is null, line: " << line;
      continue;
    }

    TestSample test_sample;
    test_sample.text = line.substr(pos + 1);
    test_sample.category_name = line.substr(0, pos);
    if (!hierarchical_classifier_->taxonomy().Has(
            test_sample.category_name, true)) {
      LOG(WARNING) << "The label is illegal, line: " << line;
      continue;
    }
    test_sample.category_id
        = hierarchical_classifier_->taxonomy().Id(test_sample.category_name);
    test_corpus_.push_back(test_sample);
  }
  fs.close();

  return true;
}

void ClassifierEvaluator::Predict() {
  size_t count = 0;
  for (size_t i = 0; i < test_corpus_.size(); ++i) {
    Instance instance;
    ExtractFeature(test_corpus_[i].text,
                   *(feature_extractor_.get()),
                   &instance);
    if (instance.Empty()) {
      continue;
    }
    hierarchical_classifier_->Predict(instance,
                                      &(test_corpus_[i].predict_result));

    // 预测单个样例后，就开始统计
    std::vector<int32_t> ancestors;
    hierarchical_classifier_->taxonomy().Ancestors(test_corpus_[i].category_id,
                                                   &ancestors);
    for (size_t j = 0; j < ancestors.size(); ++j) {
      std::string category_name
          = hierarchical_classifier_->taxonomy().Name(ancestors[j]);
      Stat(category_name,
           ancestors[j],
           hierarchical_classifier_->taxonomy().Depth(ancestors[j]),
           test_corpus_[i].predict_result);
    }
    ++count;
    if (count % 1000 == 0) {
      LOG(INFO) << "Number of samples: " << count;
    }
  }
  LOG(INFO) << "Number of samples: " << count;
}

std::string ClassifierEvaluator::Predict(const std::string& text) {
  Instance instance;
  Result predict_result;
  ExtractFeature(text, *(feature_extractor_.get()), &instance);
  hierarchical_classifier_->Predict(instance, &predict_result);
  std::string buffer = "";
  if (predict_result.size() == 0
      || predict_result.back().size() == 0) {
    StringAppendF(&buffer, "Nil\t-1");
  } else {
    StringAppendF(&buffer, "%s\t%.4f",
                  hierarchical_classifier_->taxonomy().Name(
                      predict_result.back()[0].id()).c_str(),
                  predict_result.back()[0].probability());
  }
  return buffer.c_str();
}

bool ClassifierEvaluator::Evaluate(const std::string& output_file) {
  std::ofstream fout(output_file.c_str());
  if (fout.fail()) {
    LOG(ERROR) << "Failed to open the output_file: " << output_file;
    return false;
  }

  // 微平均 P R F
  int32_t depth = hierarchical_classifier_->taxonomy().Depth();
  for (int32_t i = 1; i <= depth; ++i) {
    std::string buffer = "";
    int32_t predict_count = 0;
    int32_t standard_count = 0;
    int32_t right_count = 0;

    for (ConfusionMatrix::const_iterator citer = confusion_matrix_.begin();
         citer != confusion_matrix_.end();
         ++citer) {
      if (hierarchical_classifier_->taxonomy().Depth(citer->first) == i) {
        StringAppendF(&buffer, "%s\t%d",
                      hierarchical_classifier_->taxonomy().Name(citer->first).c_str(),
                      citer->first);
        StringAppendF(&buffer, "\tStandard Count: %d",
                      standard_category_count_[citer->first]);
        StringAppendF(&buffer, "\tPredict Count: %d",
                      predict_category_count_[citer->first]);
        StringAppendF(&buffer, "\tRight Count: %d",
                      confusion_matrix_[citer->first][citer->first]);
        StringAppendF(&buffer, "\tP: %lf",
                      confusion_matrix_[citer->first][citer->first] * 1.0 \
                      / predict_category_count_[citer->first]);
        StringAppendF(&buffer, "\tR: %lf\n",
                      confusion_matrix_[citer->first][citer->first] * 1.0 \
                     / standard_category_count_[citer->first]);

        standard_count += standard_category_count_[citer->first];
        predict_count += predict_category_count_[citer->first];
        right_count += confusion_matrix_[citer->first][citer->first];
      }
    }
    StringAppendF(&buffer, "\nDepth: %d", i);
    StringAppendF(&buffer, "\tStandard Count: %d", standard_count);
    StringAppendF(&buffer, "\tPredict Count: %d", predict_count);
    StringAppendF(&buffer, "\tRight Count: %d", right_count);
    StringAppendF(&buffer, "\tP: %lf", right_count * 1.0 / predict_count);
    StringAppendF(&buffer, "\tR: %lf\n\n", right_count * 1.0 / standard_count);

    fout.write(buffer.c_str(), buffer.size());
  }
  fout.close();

  return true;
}

bool ClassifierEvaluator::Save(const std::string& result_file) const {
  std::ofstream fout(result_file.c_str());
  if (fout.fail()) {
    LOG(ERROR) << "Failed to open the result_file: " << result_file;
    return false;
  }
  for (size_t i = 0; i < test_corpus_.size(); ++i) {
    std::string buffer = "";
    StringAppendF(&buffer, "%s\t%d\t",
                  test_corpus_[i].category_name.c_str(),
                  test_corpus_[i].category_id);
    if (test_corpus_[i].predict_result.size() == 0
        || test_corpus_[i].predict_result.back().size() == 0) {
      // TODO(fandywang): 可以输出上层类别结果
      StringAppendF(&buffer, "Nil\t-1");
    } else {
      StringAppendF(&buffer, "%s\t%d",
                    hierarchical_classifier_->taxonomy().Name(
                        test_corpus_[i].predict_result.back()[0].id()).c_str(),
                    test_corpus_[i].predict_result.back()[0].id());
    }
    StringAppendF(&buffer, "\t%s\n", test_corpus_[i].text.c_str());
    fout.write(buffer.c_str(), buffer.size());
  }

  fout.close();
  return true;
}

void ClassifierEvaluator::ExtractFeature(
    const std::string& text,
    const FeatureExtractor& feature_extractor,
    Instance* instance) const {
  Document document;
  Field* field = document.add_field();
  field->set_text(text);
  field->set_weight(1.0);

  // extract token features
  if (FLAGS_on_token_feature) {
    if (!text_miner_->ExtractTokens(&document)) {
      LOG(WARNING) << "Extract tokens failed";
    } else {
      feature_extractor.ExtractTokenFeature(document, instance);
    }
  }

  // extract keyword features
  if (FLAGS_on_keyword_feature) {
    if (!text_miner_->ExtractKeywords(&document)) {
      LOG(WARNING) << "Extract keywords failed";
    } else {
      feature_extractor.ExtractKeywordFeature(document, instance);
    }
  }

  // extract lda features
  if (FLAGS_on_lda_feature) {
    if (!text_miner_->InferTopics(&document)) {
      LOG(WARNING) << "Infer topics failed";
    } else {
      feature_extractor.ExtractTopicFeature(document, instance);
    }
  }
  // NOTE: 此处不需要对Embedding特征做 L1-normalize
  instance->L1Normalize();

  // extract embedding features
  if (FLAGS_on_embedding_feature) {
    if (!text_miner_->InferEmbedding(&document)) {
      LOG(WARNING) << "Infer embedding failed";
    } else {
      feature_extractor.ExtractEmbeddingFeature(document, instance);
    }
  }
}

void ClassifierEvaluator::Stat(const std::string& category_name,
                               const int32_t category_id,
                               const int32_t depth,
                               const Result& result) {
  if (depth == 0) return;
  standard_category_count_[category_id] += 1;

  // 第 depth 层没有预测结果
  if (result.size() < static_cast<size_t>(depth)
      || result[depth - 1].size() == 0) {
    predict_category_count_[-1] += 1;
    return;
  }

  bool right = false;
  for (size_t i = 0;
       i < result[depth - 1].size()
       && i < static_cast<size_t>(FLAGS_topk_results);
       ++i) {
    if (category_id == result[depth - 1][i].id()) {
      right = true;
      break;
    }
  }
  if (right) {
    confusion_matrix_[category_id][category_id] += 1;
    predict_category_count_[category_id] += 1;
  } else {
    confusion_matrix_[category_id][result[depth - 1][0].id()] += 1;
    predict_category_count_[result[depth - 1][0].id()] += 1;
  }
}

}  // namespace text_analysis
}  // namespace qzap
