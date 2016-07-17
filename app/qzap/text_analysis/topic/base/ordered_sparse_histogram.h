// Copyright (c) 2013 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_ORDERED_SPARSE_HISTOGRAM_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_ORDERED_SPARSE_HISTOGRAM_H_

#include <string>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/topic/base/cgo_types.h"
#include "app/qzap/text_analysis/topic/base/lda.pb.h"

namespace qzap {
namespace text_analysis {
namespace base {

// unassigned topic id
const int32_t kUnassignedTopic = -1;

class DenseTopicHistogram;

// OrderedSparseHistogram is a C++ wrapper of the C type
// ::OrderedSparseHistogram, which, defined in cgo_types.h, has the only
// purpose of data exchange between Go and C/C++, whereas
// OrderedSparseHistogram has related methods.
class OrderedSparseHistogram {
 public:
  class ConstIterator {
   public:
    explicit ConstIterator(const OrderedSparseHistogram& parent) :
        parent_(parent), index_(0) {}

    bool Done() const {
      return (parent_.topic_histogram_ == NULL ||
              index_ >= parent_.topic_histogram_->topics.len);
    }

    void Next() {
      CHECK(!Done());
      ++index_;
    }

    int32_t Topic() const {
      CHECK(!Done());
      return static_cast<int32_t*>(
          parent_.topic_histogram_->topics.array)[index_];
    }

    int32_t Count() const {
      CHECK(!Done());
      return static_cast<int64_t*>(
          parent_.topic_histogram_->counts.array)[index_];
    }

   private:
    const OrderedSparseHistogram& parent_;
    uint32_t index_;
  };  // class ConstIterator

 public:
  // There are two supposed usages of OrderedSparseHistogram: the histogram
  // should be allocated at construction time, or the histogram is already
  // there in Go/C.  The former happens in representing the model or in
  // inference, and the latter happends in training, when
  // OrderedSparseHistogram represents N(t|d) We define constructors for the
  // cases respectively
  explicit OrderedSparseHistogram(int32_t num_topics);
  OrderedSparseHistogram(const ::OrderedSparseHistogram* hist,
                         int32_t num_topics);
  ~OrderedSparseHistogram();

  void ParseFrom(const SparseTopicHistogramPB& pb, int32_t num_topics);
  void SerializeTo(SparseTopicHistogramPB* pb) const;
  void SerializeToPBString(std::string* s) const;
  void ParseFrom(const DenseTopicHistogram& dense);

  // The CType will be called by C/Go during LDA training
  ::OrderedSparseHistogram* CType() {
    return topic_histogram_;
  }

  void IncrementTopic(int32_t topic, int64_t count);
  void DecrementTopic(int32_t topic, int64_t count);

  int64_t Count(int32_t topic) const;

  int32_t Length() const {
    return topic_histogram_ == NULL ? 0 : topic_histogram_->topics.len;
  }

  int32_t NumTopics() const { return num_topics_; }

  double SparseRatio() const {
    if (NumTopics() == 0) {
      LOG(ERROR) << "NumTopics = 0";
      return 0;
    }
    return static_cast<double>(Length()) / NumTopics();
  }

  bool IsOrdered() const;

  // fmt: topic:count topic:count ...
  void AppendAsString(std::string* s) const;

 private:
  void Clear();
  // the data allocate function of topic_histogram_
  void IncreaseCapacity();
  // just for unittest
  uint32_t Capacity() const {
    return topic_histogram_ == NULL ? 0 : topic_histogram_->topics.cap;
  }

  FRIEND_TEST(OrderedSparseHistogramTest, IncreaseCapacity);

  friend class ConstIterator;

  ::OrderedSparseHistogram* topic_histogram_;

  int32_t num_topics_;  // number of all topics

  // whether class OrderedSparseHistogram should free topic_histogram_
  // true: the topic_histogram_ should be allocated and deallocated inside
  // false: the topic_histogram_ should be allocated at construction time,
  // and deallocated by C, which will get topic_histogram_ using CType().
  bool own_memory_;
};  // class OrderedSparseHistogram

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_ORDERED_SPARSE_HISTOGRAM_H_
