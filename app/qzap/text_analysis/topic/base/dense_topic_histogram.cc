// Copyright (c) 2011 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/base/dense_topic_histogram.h"

#include "app/qzap/common/base/string_utility.h"

namespace qzap {
namespace text_analysis {
namespace base {

void DenseTopicHistogram::AppendAsString(std::string* s) const {
  for (size_t i = 0; i < count_.size(); ++i) {
    StringAppendF(s, "%d ", count_[i]);
  }
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

