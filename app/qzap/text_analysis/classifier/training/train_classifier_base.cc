// Copyright (c) 2012 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/classifier/training/train_classifier_base.h"

#include "app/qzap/common/base/string_utility.h"

namespace qzap {
namespace text_analysis {

using std::string;
using std::vector;

bool TrainClassifierBase::ParseTrainSampleString(
    const std::string& str, std::vector<std::string>* category,
    Instance* feature, std::string* comments) {
  category->clear();
  feature->Clear();
  comments->clear();

  vector<string> fields;
  SplitString(str, "\t", &fields);
  if (fields.size() < 3u) {
    LOG(WARNING) << "Format of training sample string is error: "
        << "category\\tfeature\\tcomments";
    return false;
  }

  TrimString(&fields[0]);
  SplitStringByString(fields[0], "--", category);
  if (category->empty()) {
    LOG(WARNING) << "Format of training sample string is error: "
        << "level-1-category--level-2-category--....--level-n-category";
    return false;
  }
  for (size_t i = 1; i < category->size(); ++i) {
    (*category)[i] = (*category)[i - 1] + "--" + (*category)[i];
  }

  feature->ParseFrom(fields[1]);
  if (feature->Empty()) {
    LOG(WARNING) << "Feature is empty";
  }

  *comments = JoinString(fields.begin() + 2, fields.end(), "\t");
  return true;
}

}  // namespace text_analysis
}  // namespace qzap

