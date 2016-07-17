// Copyright (c) 2012 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// 文本分类评测工具

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_EVALUATION_CLASSIFIER_EVALUATOR_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_EVALUATION_CLASSIFIER_EVALUATOR_H_

#include <tr1/unordered_map>
#include <string>
#include <vector>

#include "app/qzap/common/base/scoped_ptr.h"
#include "thirdparty/gflags/gflags.h"

DECLARE_string(segmenter_data_dir);
DECLARE_string(text_miner_resource_config_file);
DECLARE_double(hierarchical_classifier_threshold);

namespace qzap {
namespace text_analysis {

class Label;
class HierarchicalClassifier;
class Instance;
class TextMinerResource;
class TextMiner;

class FeatureExtractor;

class ClassifierEvaluator {
 public:
  ClassifierEvaluator();
  ~ClassifierEvaluator();

  bool LoadClassifierModel(const std::string& classifier_model_dir,
                           const std::string& feature_vocabulary_file);
  bool LoadTestCorpus(const std::string& test_data_file);
  void Predict();  // 预测测试样例
  std::string Predict(const std::string& text);  // 对单文本进行分类
  bool Evaluate(const std::string& output_file);  // 计算并输出评测指标
  bool Save(const std::string& result_file) const;  // 输出测试样例及预测结果

 private:
  typedef std::vector<std::vector<Label> > Result;
  typedef std::tr1::unordered_map<int32_t, uint32_t> CategoryCount;
  typedef std::tr1::unordered_map<int32_t, CategoryCount> ConfusionMatrix;

  // 特征抽取器
  void ExtractFeature(const std::string& text,
                      const FeatureExtractor& feature_extractor,
                      Instance* instance) const;

  // 比较标准类别和预测类别
  void Stat(const std::string& category_name,
            const int32_t category_id,
            const int32_t depth,
            const Result& result);

  // 测试样例
  struct TestSample {
    std::string text;  // 测试文本
    int32_t category_id;  // 标准类别id
    std::string category_name;  // 标准类别名称
    Result predict_result;  // 预测结果
  };

  scoped_ptr<TextMinerResource> text_miner_resource_;
  scoped_ptr<TextMiner> text_miner_;

  scoped_ptr<HierarchicalClassifier> hierarchical_classifier_;  // 层次分类器
  scoped_ptr<FeatureExtractor> feature_extractor_;  // 特征提取器

  std::vector<TestSample> test_corpus_;  // 测试语料库

  CategoryCount standard_category_count_;  // 标准类别结果分布
  CategoryCount predict_category_count_;  // 预测类别结果分布
  ConfusionMatrix confusion_matrix_;  // 预测结果统计
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_EVALUATION_CLASSIFIER_EVALUATOR_H_
