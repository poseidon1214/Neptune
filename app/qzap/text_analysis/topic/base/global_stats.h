// Copyright (c) 2011 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_GLOBAL_STATS_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_GLOBAL_STATS_H_

#include <fstream>
#include <string>

#include "app/qzap/text_analysis/topic/base/dense_topic_histogram.h"

namespace qzap {
namespace text_analysis {
namespace base {

// GlobalStats stores the global topic histogram.
class GlobalStats {
 public:
  GlobalStats() {}
  ~GlobalStats() {}

  // local file or xfs file
  bool Load(std::ifstream* fin);
  bool Save(std::ofstream* fout) const;

  void ParseFrom(const DenseTopicHistogram& dense) {
    global_topic_histogram_ = dense;
  }

  const DenseTopicHistogram& GetTopicHistogram() const {
    return global_topic_histogram_;
  }

  DenseTopicHistogram& GetTopicHistogram() {
    return global_topic_histogram_;
  }

  int32_t NumTopics() const {
    return global_topic_histogram_.NumTopics();
  }

  // Output gobal_topic_histograms into human readable format.
  // fmt: count count ... \n
  void AppendAsString(std::string* s) const;

 private:
  // global_histogram_[k] is the number of words in the training
  // corpus that are assigned by topic k.
  DenseTopicHistogram global_topic_histogram_;
};  // class GlobalStats

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_GLOBAL_STATS_H_
