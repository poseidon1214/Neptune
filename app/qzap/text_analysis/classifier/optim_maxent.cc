// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/classifier/optim_maxent.h"

#include <float.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"

namespace qzap {
namespace text_analysis {

using std::string;
using std::vector;

OptimMaxEnt::OptimMaxEnt() { weights_.set_empty_key(-1); }

OptimMaxEnt::~OptimMaxEnt() { weights_.clear(); }

bool OptimMaxEnt::LoadFromDir(const std::string& dir) {
  if (dir.empty()) {
    LOG(ERROR) << "directory path string is empty";
    return false;
  }

  std::string dirname = dir + "/";
  bool ok = taxonomy_.LoadFromTextFile(dirname + "taxonomy");
  if (!ok) {
    LOG(ERROR) << "failed to load taxonomy";
    return false;
  }

  ok = LoadWeights(dirname + "maxent");
  if (!ok) {
    LOG(ERROR) << "failed to load ME_Model";
    return false;
  }

  if (static_cast<size_t>(taxonomy_.NumNodes()) != weights_.size() + 1u) {
    LOG(WARNING) << "OptimMaxEnt " << taxonomy_.Name(taxonomy_.Root())
        << ": taxonomy " << taxonomy_.NumNodes() - 1
        << " vs. model " << weights_.size();
  }
  return true;
}

void OptimMaxEnt::Predict(const Instance& instance, Result* result) const {
  result->clear();
  result->push_back(std::vector<Label>());
  if (taxonomy_.NumNodes() == 2) {
    int32_t id = taxonomy_.Children(taxonomy_.Root()).front();
    result->front().push_back(Label());
    result->front().back().set_id(id);
    result->front().back().set_probability(1.0);
    return;
  }

  std::vector<double> prob;
  CalculateProbabilities(instance, &prob);
  SparseMatrix::const_iterator ci = weights_.begin();
  result->front().resize(prob.size());
  for (size_t i = 0; i < prob.size(); ++i) {
    result->front()[i].set_id(ci->first);
    result->front()[i].set_probability(prob[i]);
    ++ci;
  }
  std::sort(result->front().begin(), result->front().end(),
            CategoryProbabilityGreater);
}

bool OptimMaxEnt::LoadWeights(const std::string& filepath) {
  weights_.clear();

  std::ifstream fin(filepath.c_str());
  if (fin.fail()) {
    LOG(ERROR) << "failed to open file '" << filepath << "'";
    return false;
  }

  string line;
  int32_t line_id = 0;
  while (std::getline(fin, line) > 0) {
    ++line_id;
    vector<string> fields;
    SplitString(line, "\t", &fields);

    if (fields.size() != 3u) {
      LOG(WARNING) << "line " << line_id << ": \"" << line
          << "\", has not enough fields";
      continue;
    }
    if (!taxonomy_.Has(fields[0], false)) {
      LOG(ERROR) << "category " << fields[0] << "not found in taxonomy";
      continue;
    }

    int32_t category_id = taxonomy_.Id(fields[0]);
    if (weights_.find(category_id) == weights_.end()) {
      weights_[category_id].set_empty_key(-1);
    }

    int32_t feature_id;
    StringToNumeric(fields[1], &feature_id);
    double weight;
    StringToNumeric(fields[2], &weight);
    weights_[category_id][feature_id] = weight;
  }
  fin.close();
  return true;
}

void OptimMaxEnt::CalculateProbabilities(
    const Instance& instance, std::vector<double>* prob) const {
  prob->clear();
  prob->reserve(weights_.size());

  double max_sum = -FLT_MAX;
  uint32_t num_features = instance.NumFeatures();
  for (SparseMatrix::const_iterator i = weights_.begin();
       i != weights_.end(); ++i) {
    double sum = 0;
    std::ostringstream ostr;
    if (instance.IsDebug()) {
      ostr << "CategoryId:" << i->first;
    }
    for (uint32_t j = 0; j < num_features; ++j) {
      google::dense_hash_map<int32_t, double>::const_iterator it =
          i->second.find(static_cast<int32_t>(instance.IdAt(j)));
      if (it != i->second.end()) {
        sum += instance.WeightAt(j) * it->second;
        if (instance.IsDebug()) {
          ostr << " " << instance.IdAt(j)
               << " " << instance.WeightAt(j)
               << " " << it->second << "|";
        }
      }
    }
    if (instance.IsDebug()) {
      ostr << " Sum:" << sum;
      instance.AddDebugString(ostr.str());
    }
    prob->push_back(sum);
    if (sum > max_sum) {
      max_sum = sum;
    }
  }

  // to avoid overflow
  const double kMagic = 700.0;
  double offset = max_sum > kMagic ? max_sum - kMagic : 0;

  double prob_sum = 0;
  for (size_t i = 0; i < prob->size(); ++i) {
    (*prob)[i] = exp((*prob)[i] - offset);
    prob_sum += (*prob)[i];
  }

  for (size_t i = 0; i < prob->size(); ++i) {
    (*prob)[i] /= prob_sum;
  }
}

}  // namespace text_analysis
}  // namespace qzap

