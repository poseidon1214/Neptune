// Copyright (c) 2012 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "app/qzap/text_analysis/classifier/evaluation/classifier_evaluator.h"

DECLARE_string(segmenter_data_dir);
DECLARE_string(text_miner_resource_config_file);

DECLARE_string(feature_vocab_path);
DECLARE_double(hierarchical_classifier_threshold);

DECLARE_int32(peacock_cache_size_mb);  // default = 5 * 1024
DECLARE_int32(peacock_num_markov_chains);  // default = 5
DECLARE_int32(peacock_total_iterations);  // default = 15
DECLARE_int32(peacock_burn_in_iterations);  // default = 10
DECLARE_int32(peacock_topic_top_k);
DECLARE_int32(peacock_topic_word_top_k);

DEFINE_string(classifier_model_dir, "", "the classifier model directory");
DEFINE_string(feature_vocabulary_file, "dict.feature_vocabulary",
              "the binary feature vocabulary file");
DEFINE_string(test_corpus_file, "", "the test corpus filename");
DEFINE_string(evaluation_result_file, "", "the evaluation result filename");
DEFINE_string(test_corpus_output_file, "", "the test corpus output filename");

namespace qzap {
namespace text_analysis {

int main(int32_t argc, char** argv) {
  ClassifierEvaluator classifier_evaluator;
  CHECK(classifier_evaluator.LoadClassifierModel(FLAGS_classifier_model_dir,
                                                 FLAGS_feature_vocabulary_file));
  CHECK(classifier_evaluator.LoadTestCorpus(FLAGS_test_corpus_file));

  classifier_evaluator.Predict();
  CHECK(classifier_evaluator.Evaluate(FLAGS_evaluation_result_file));
  CHECK(classifier_evaluator.Save(FLAGS_test_corpus_output_file));

  return 0;
}

}  // namespace text_analysis
}  // namespace qzap

int main(int32_t argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);

  return qzap::text_analysis::main(argc, argv);
}

