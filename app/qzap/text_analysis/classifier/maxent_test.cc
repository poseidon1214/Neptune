// Copyright (c) 2011 Tencent Inc.
// Author: ZhiQiang Chen (lucienchen@tencent.com)

#include "app/qzap/text_analysis/classifier/maxent.h"

#include <fstream>
#include <iostream>
#include "thirdparty/gtest/gtest.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/classifier/classifier_base.h"

namespace qzap {
namespace text_analysis {

using std::string;
using std::vector;
using std::endl;

const char* kModelDir = "testdata/test_hierarchical_classifier_model/11";
const char* kInstanceFile = "testdata/test_classifier_instances";

TEST(MaxentTest, Load) {
  MaxEnt maxent;
  EXPECT_TRUE(maxent.LoadFromDir(kModelDir));
  EXPECT_FALSE(maxent.LoadFromDir("."));
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
    vector<string> str_vec;
    SplitString(items[1], " ", &str_vec);

    if (str_vec.empty()) {
      LOG(WARNING) << "empty feature: " << line;
    }

    instances->push_back(Instance());
    Instance& instance = instances->back();
    for (size_t i = 0; i < str_vec.size(); ++i) {
      vector<string> par;
      SplitString(str_vec[i], ":", &par);
      if (par.size() != 2u) {
        LOG(ERROR) << "pair-count " << line;
        continue;
      }

      int32_t id;
      if (!StringToNumeric(par[0], &id)) {
        LOG(ERROR) << "StringToNumber(id) " << line;
        continue;
      }
      double val;
      if (!StringToNumeric(par[1], &val)) {
        LOG(ERROR) << "StringToNumber(value) " << line;
        continue;
      }
      instance.AddFeature(id, val);
    }
  }
  return true;
}

TEST(MaxentTest, Predict) {
  MaxEnt maxent;
  ASSERT_TRUE(maxent.LoadFromDir(kModelDir));

  vector<vector<string> > labels;
  vector<Instance> instances;
  vector<string> comments;
  ASSERT_TRUE(LoadTestInstances(kInstanceFile, &labels,
                                &instances, &comments));
  ASSERT_EQ(5u, labels.size());
  ASSERT_EQ(5u, instances.size());
  ASSERT_EQ(5u, comments.size());

  MaxEnt::Result result;
  for (size_t i = 0; i < labels.size(); ++i) {
    maxent.Predict(instances[i], &result);
    EXPECT_EQ(6U, result.front().size());
    EXPECT_EQ(labels[i].back(),
              maxent.taxonomy().Name(result.front().front().id()));
  }
}

}  // namespace text_analysis
}  // namespace qzap

