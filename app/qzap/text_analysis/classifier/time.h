// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TIME_H_
#define APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TIME_H_

#include <stack>
#include "app/qzap/common/utility/time_utility.h"
#include "thirdparty/glog/logging.h"

namespace qzap {
namespace text_analysis {

class Time {
 public:
  Time() {}

  ~Time() {
    if (!stamps_.empty()) {
      LOG(WARNING) << "time stamp stack is not empty: "
          << stamps_.size();
    }
  }

  void Tic() {
    stamps_.push(GetCurrentTimeMillis());
  }

  // unit: millisecond
  double Toc() {
    if (stamps_.empty()) {
      LOG(ERROR) << "Tic & Toc not match";
      return -1.0;
    }
    int64_t diff = GetCurrentTimeMillis() - stamps_.top();
    stamps_.pop();
    return diff / 1000.0;
  }

  void Clear() {
    while (!stamps_.empty()) {
      stamps_.pop();
    }
  }

  size_t StackDepth() const {
    return stamps_.size();
  }

 private:
  std::stack<int64_t> stamps_;
};  // class Time

// simplify use
void Tic();
double Toc();

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_CLASSIFIER_TIME_H_
