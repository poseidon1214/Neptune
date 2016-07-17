// Copyright (c) 2011 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_DENSE_TOPIC_HISTOGRAM_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_DENSE_TOPIC_HISTOGRAM_H_

#include <string>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/text_analysis/topic/base/lda.pb.h"

namespace qzap {
namespace text_analysis {
namespace base {

// This is the internal data structure of DenseTopicHistogramPB
class DenseTopicHistogram {
 public:
  void Resize(int32_t topic_count) {
    count_.resize(topic_count);
    count_.assign(topic_count, 0);
  }

  void ParseFrom(const DenseTopicHistogramPB& pb) {
    Resize(pb.count_size());
    for (int32_t i = 0; i < pb.count_size(); ++i) {
      count_[i] = pb.count(i);
    }
  }

  void SerializeTo(DenseTopicHistogramPB* pb) const {
    pb->Clear();
    for (size_t i = 0; i < count_.size(); ++i) {
      pb->add_count(count_[i]);
    }
  }

  bool ParseFromPBString(const std::string& s) {
    DenseTopicHistogramPB pb;
    if (!pb.ParseFromString(s)) {
      return false;
    }
    ParseFrom(pb);
    return true;
  }

  void SerializeToPBString(std::string* s) const {
    DenseTopicHistogramPB pb;
    SerializeTo(&pb);
    pb.SerializeToString(s);
  }

  int32_t NumTopics() const {
    return static_cast<int32_t>(count_.size());
  }

  int32_t operator[](int32_t i) const {
    return count_[i];
  }

  int32_t& operator[](int32_t i) {
    return count_[i];
  }

  // fmt: count count ...
  void AppendAsString(std::string* s) const;

 private:
  std::vector<int32_t> count_;
};  // class DenseTopicHistogram

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_DENSE_TOPIC_HISTOGRAM_H_
