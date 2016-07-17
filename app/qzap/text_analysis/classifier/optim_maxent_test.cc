// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/optim_maxent.h"

#include <fstream>

#include "thirdparty/gtest/gtest.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/classifier/maxent.h"
#include "app/qzap/text_analysis/classifier/time.h"

namespace qzap {
namespace text_analysis {

using std::string;
using std::vector;

const char* kInstanceFile = "testdata/test_classifier_instances";
const char* kModelDir = "testdata/test_hierarchical_classifier_model/11";

TEST(OptimMaxEnt, LoadFromDir) {
  OptimMaxEnt optim_maxent;
  EXPECT_TRUE(optim_maxent.LoadFromDir(kModelDir));
  EXPECT_FALSE(optim_maxent.LoadFromDir("."));
}

bool LoadTestInstances(const string& filepath,
                       vector<vector<string> >* labels,
                       vector<Instance>* instances,
                       vector<string>* comments) {
  std::ifstream is(filepath.c_str());
  string line;
  while (std::getline(is, line)) {
    string::size_type i = line.find('\t');
    if (i == string::npos) {
      LOG(ERROR) << "item-count " << line;
      continue;
    }
    string::size_type j = line.find('\t', i + 1);
    if (j == string::npos) {
      LOG(ERROR) << "item-count " << line;
      continue;
    }

    vector<string> items;
    items.push_back(line.substr(0, i));
    items.push_back(line.substr(i + 1, j - i - 1));
    items.push_back(line.substr(j + 1));

    // label
    labels->push_back(vector<string>());
    SplitString(items[0], ".", &(labels->back()));

    // comment
    comments->push_back(string());
    RTrimString(&items[2]);
    comments->back() = items[2];

    // feature
    instances->push_back(Instance());
    instances->back().ParseFrom(items[1]);
  }
  return true;
}

TEST(OptimMaxEnt, Predict) {
  MaxEnt maxent;
  OptimMaxEnt optim_maxent;
  ASSERT_TRUE(maxent.LoadFromDir(kModelDir));
  ASSERT_TRUE(optim_maxent.LoadFromDir(kModelDir));

  vector<vector<string> > labels;
  vector<Instance> instances;
  vector<string> comments;
  ASSERT_TRUE(LoadTestInstances(kInstanceFile, &labels,
                                &instances, &comments));
  ASSERT_EQ(5u, labels.size());
  ASSERT_EQ(5u, instances.size());
  ASSERT_EQ(5u, comments.size());

  double t = 0;
  double optim_t = 0;

  MaxEnt::Result result;
  OptimMaxEnt::Result optim_result;
  for (size_t i = 0; i < labels.size(); ++i) {
    Tic();
    maxent.Predict(instances[i], &result);
    t += Toc();

    Tic();
    optim_maxent.Predict(instances[i], &optim_result);
    optim_t += Toc();

    ASSERT_EQ(result.size(), optim_result.size());
    for (size_t j = 0; j < result.size(); ++j) {
      ASSERT_EQ(result[j].size(), optim_result[j].size());
      for (size_t k = 0; k < result[j].size(); ++k) {
        EXPECT_EQ(result[j][k].id(), optim_result[j][k].id());
        EXPECT_NEAR(result[j][k].probability(),
                    optim_result[j][k].probability(), 0.001);
      }
    }
  }

  LOG(INFO) << "speedup ratio: " << t / optim_t;
}

}  // namespace text_analysis
}  // namespace qzap

