// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/training/train_hierarchical_classifier.h"
#include "thirdparty/gflags/gflags.h"

DEFINE_string(taxonomy_file, "", "file path of taxonomy");
DEFINE_string(sample_file, "", "file path of training samples");
DEFINE_string(model_dir, "", "taxonomy classifier model directory to save");

namespace qzap {
namespace text_analysis {

int main(int argc, char** argv) {
  if (!google::ParseCommandLineFlags(&argc, &argv, true)) {
    LOG(ERROR) << "parse command line failed";
    return -1;
  }

  TaxonomyHierarchy taxonomy;
  CHECK(taxonomy.LoadFromTextFile(FLAGS_taxonomy_file)) <<
      "failed to load taxonomy file from '" << FLAGS_taxonomy_file << "'";
  TrainHierarchicalClassifier hierarchical_classifier_trainer;
  hierarchical_classifier_trainer.Train(FLAGS_sample_file, taxonomy);
  hierarchical_classifier_trainer.SaveToDir(FLAGS_model_dir);
  return 0;
}

}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  return qzap::text_analysis::main(argc, argv);
}

