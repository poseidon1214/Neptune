// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/training/train_maxent.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <vector>

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/thirdparty/maxent/maxent.h"

DEFINE_int32(maxent_traing_heldout, 0,
             "heldout number for maxent training. "
             "pls refer to ME_Model::set_heldout.");
DEFINE_string(maxent_training_regularizer_type, "L1",
              "regularizer type of maxent training: L1 vs. L2");
DEFINE_double(maxent_training_l1_regularizer, 0,
              "L1-regularizer factor for maxent training");
DEFINE_double(maxent_training_l2_regularizer, 0,
              "L2-regularizer factor for maxent training");
DEFINE_string(maxent_traing_optim_method, "LBFGS",
              "optimization method of maxent: LBFGS vs. SGD."
              "Note: SGD is available only for L1-regularization");
DEFINE_int32(maxent_traing_sgd_optim_iterations, 30,
             "SGB optimization iterations for maxent training.");

namespace qzap {
namespace text_analysis {

using std::string;
using std::vector;

TrainMaxEnt::TrainMaxEnt() : impl_(new MaxEntImpl) {}

TrainMaxEnt::~TrainMaxEnt() {
  delete impl_;
  impl_ = NULL;
}

void TrainMaxEnt::SaveToDir(const std::string& dir) const {
  CHECK(!dir.empty());
  string dirname = dir + "/";
  if (access(dirname.c_str(), R_OK) != 0) {
    CHECK_EQ(0, mkdir(dirname.c_str(), 0777));
  }
  taxonomy_.SaveToTextFile(dirname + "taxonomy");
  CHECK(impl_->save_to_file(dirname + "maxent"));
}

void TrainMaxEnt::Train(const std::string& sample_filepath,
                        const TaxonomyHierarchy& taxonomy) {
  impl_->clear();

  // taxonomy
  CHECK_EQ(1, taxonomy.Depth());
  taxonomy_ = taxonomy;

  // prepare training samples
  std::ifstream fin(sample_filepath.c_str());
  if (fin.fail()) {
    LOG(ERROR) << "Open sample file " << sample_filepath << " failed.";
    return;
  }

  string line;
  int32_t line_id = 0;
  vector<string> category;
  Instance feature;
  string comments;
  while (std::getline(fin, line)) {
    ++line_id;
    if (!ParseTrainSampleString(line, &category, &feature, &comments)) {
      LOG(WARNING) << "failed to parse from line " << line_id << ": " << line;
      continue;
    }

    for (size_t i = 0; i < category.size(); ++i) {
      if (taxonomy_.Has(category[i], false)) {
        MaxEntSample sample;
        sample.label = category[i];
        for (uint32_t j = 0; j < feature.NumFeatures(); ++j) {
          sample.add_feature(ConvertToString(feature.IdAt(j)),
                             feature.WeightAt(j));
        }
        impl_->add_training_sample(sample);
        break;
      }
    }
  }
  fin.close();

  // train
  impl_->set_heldout(FLAGS_maxent_traing_heldout);
  if (FLAGS_maxent_training_regularizer_type == "L2") {
    impl_->use_l2_regularizer(FLAGS_maxent_training_l2_regularizer);
  } else {
    impl_->use_l1_regularizer(FLAGS_maxent_training_l1_regularizer);
  }
  if (FLAGS_maxent_traing_optim_method == "SGD") {
    impl_->use_SGD(FLAGS_maxent_traing_sgd_optim_iterations);
  }

  impl_->train();

  if (taxonomy_.NumNodes() != impl_->num_classes() + 1) {
    LOG(WARNING) << "MaxEnt " << taxonomy_.Name(taxonomy_.Root())
        << " taxonomy " << taxonomy_.NumNodes() - 1
        << " vs. classifier " << impl_->num_classes();
  }
}

}  // namespace text_analysis
}  // namespace qzap

