// Copyright (c) 2013 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/base/ordered_sparse_histogram.h"

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/topic/base/dense_topic_histogram.h"

namespace qzap {
namespace text_analysis {
namespace base {

OrderedSparseHistogram::OrderedSparseHistogram(int32_t num_topics)
  : topic_histogram_(::NewOrderedSparseHistogram(0)),
    num_topics_(num_topics),
    own_memory_(true) {}

OrderedSparseHistogram::OrderedSparseHistogram(
    const ::OrderedSparseHistogram* hist, int32_t num_topics)
  : topic_histogram_(::NewOrderedSparseHistogram(hist->topics.len)),
    num_topics_(num_topics),
    own_memory_(false) {
  if (hist->topics.array != NULL) {
    std::copy(static_cast<int32_t*>(hist->topics.array),
              static_cast<int32_t*>(hist->topics.array) + hist->topics.len,
              static_cast<int32_t*>(topic_histogram_->topics.array));
    std::copy(static_cast<int64_t*>(hist->counts.array),
              static_cast<int64_t*>(hist->counts.array) + hist->counts.len,
              static_cast<int64_t*>(topic_histogram_->counts.array));
  }
}

OrderedSparseHistogram::~OrderedSparseHistogram() {
  Clear();
}

void OrderedSparseHistogram::ParseFrom(
    const SparseTopicHistogramPB& pb, int32_t num_topics) {
  Clear();
  own_memory_ = true;
  topic_histogram_ = ::NewOrderedSparseHistogram(0);
  num_topics_ = num_topics;
  for (int32_t i = 0; i < pb.nonzero_size(); ++i) {
    IncrementTopic(pb.nonzero(i).topic(), pb.nonzero(i).count());
  }
}

void OrderedSparseHistogram::SerializeTo(
    SparseTopicHistogramPB* pb) const {
  pb->Clear();
  for (uint32_t i = 0; i < topic_histogram_->topics.len; ++i) {
    if (i > 0) {
      CHECK(((int64_t*)(topic_histogram_->counts.array))[i - 1] >=
            ((int64_t*)(topic_histogram_->counts.array))[i]);
    }
    SparseTopicHistogramPB::NonZero* nonzero = pb->add_nonzero();
    nonzero->set_topic(
        static_cast<int32_t*>(topic_histogram_->topics.array)[i]);
    nonzero->set_count(
        static_cast<int64_t*>(topic_histogram_->counts.array)[i]);
  }
}

void OrderedSparseHistogram::SerializeToPBString(std::string* s) const {
  SparseTopicHistogramPB pb;
  SerializeTo(&pb);
  pb.SerializeToString(s);
}

void OrderedSparseHistogram::ParseFrom(
    const DenseTopicHistogram& dense) {
  Clear();
  own_memory_ = true;
  topic_histogram_ = ::NewOrderedSparseHistogram(0);
  num_topics_ = dense.NumTopics();

  for (int32_t i = 0; i < num_topics_; ++i) {
    if (dense[i] > 0) {
      IncrementTopic(i, dense[i]);
    }
  }
}

void OrderedSparseHistogram::IncrementTopic(int32_t topic, int64_t count) {
  CHECK(topic < NumTopics() && topic >= 0 && count > 0
        && topic_histogram_ != NULL);

  uint32_t i = 0;
  while (i < topic_histogram_->topics.len &&
         static_cast<int32_t*>(topic_histogram_->topics.array)[i] != topic) {
    ++i;
  }
  if (i < topic_histogram_->topics.len) {  // found
    static_cast<int64_t*>(topic_histogram_->counts.array)[i] += count;
  } else {  // add a new topic
    if (topic_histogram_->topics.len == topic_histogram_->topics.cap) {
      // allocated memory is insufficient
      CHECK(topic_histogram_->counts.len == topic_histogram_->counts.cap);
      IncreaseCapacity();
    }

    static_cast<int32_t*>(topic_histogram_->topics.array)[i] = topic;
    static_cast<int64_t*>(topic_histogram_->counts.array)[i] = count;
    ++topic_histogram_->topics.len;
    ++topic_histogram_->counts.len;
  }

  // ensures that topics are sorted by counts.
  int32_t tmp_topic = static_cast<int32_t*>(topic_histogram_->topics.array)[i];
  int64_t tmp_count = static_cast<int64_t*>(topic_histogram_->counts.array)[i];
  while (i > 0 && tmp_count >
         static_cast<int64_t*>(topic_histogram_->counts.array)[i - 1]) {
    static_cast<int32_t*>(topic_histogram_->topics.array)[i] =
        static_cast<int32_t*>(topic_histogram_->topics.array)[i - 1];
    static_cast<int64_t*>(topic_histogram_->counts.array)[i] =
        static_cast<int64_t*>(topic_histogram_->counts.array)[i - 1];
    --i;
  }
  static_cast<int32_t*>(topic_histogram_->topics.array)[i] = tmp_topic;
  static_cast<int64_t*>(topic_histogram_->counts.array)[i] = tmp_count;
}

void OrderedSparseHistogram::DecrementTopic(int32_t topic, int64_t count) {
  CHECK(topic < NumTopics() && topic >= 0 && count > 0
        && topic_histogram_ != NULL);

  uint32_t i = 0;
  while (i < topic_histogram_->topics.len &&
         static_cast<int32_t*>(topic_histogram_->topics.array)[i] != topic)  {
    ++i;
  }
  CHECK(i < topic_histogram_->topics.len &&
        static_cast<int32_t*>(topic_histogram_->topics.array)[i] == topic &&
        static_cast<int64_t*>(topic_histogram_->counts.array)[i] >= count);

  static_cast<int64_t*>(topic_histogram_->counts.array)[i] -= count;
  if (static_cast<int64_t*>(topic_histogram_->counts.array)[i] == 0) {
    static_cast<int32_t*>(topic_histogram_->topics.array)[i] = kUnassignedTopic;
  }

  // ensures that topics are sorted by counts.
  int32_t tmp_topic = static_cast<int32_t*>(topic_histogram_->topics.array)[i];
  int64_t tmp_count = static_cast<int64_t*>(topic_histogram_->counts.array)[i];
  while (i + 1 < topic_histogram_->topics.len && tmp_count <
         static_cast<int64_t*>(topic_histogram_->counts.array)[i + 1]) {
    static_cast<int32_t*>(topic_histogram_->topics.array)[i] =
        static_cast<int32_t*>(topic_histogram_->topics.array)[i + 1];
    static_cast<int64_t*>(topic_histogram_->counts.array)[i] =
        static_cast<int64_t*>(topic_histogram_->counts.array)[i + 1];
    ++i;
  }
  static_cast<int32_t*>(topic_histogram_->topics.array)[i] = tmp_topic;
  static_cast<int64_t*>(topic_histogram_->counts.array)[i] = tmp_count;
  if (tmp_topic == kUnassignedTopic) {
    --topic_histogram_->topics.len;
    --topic_histogram_->counts.len;
  }
}

int64_t OrderedSparseHistogram::Count(int32_t topic) const {
  CHECK(topic >= 0 && topic < NumTopics());

  if (topic_histogram_ != NULL) {
    for (uint32_t i = 0; i < topic_histogram_->topics.len; ++i) {
      if (static_cast<int32_t*>(topic_histogram_->topics.array)[i] == topic) {
        return static_cast<int64_t*>(topic_histogram_->counts.array)[i];
      }
    }
  }

  return 0;
}

bool OrderedSparseHistogram::IsOrdered() const {
  if (topic_histogram_ != NULL) {
    for (uint32_t i = 1; i < topic_histogram_->counts.len; ++i) {
      if (static_cast<int64_t*>(topic_histogram_->counts.array)[i] >
          static_cast<int64_t*>(topic_histogram_->counts.array)[i - 1]) {
        return false;
      }
    }
  }
  return true;
}

void OrderedSparseHistogram::AppendAsString(std::string* s) const {
  for (ConstIterator it(*this); !it.Done(); it.Next()) {
    StringAppendF(s, "%d:%d ", it.Topic(), it.Count());
  }
}

void OrderedSparseHistogram::Clear() {
  if (own_memory_) {
    ::DeleteOrderedSparseHistogram(topic_histogram_);
  }
  topic_histogram_ = NULL;
}

void OrderedSparseHistogram::IncreaseCapacity() {
  topic_histogram_->topics.cap =
      topic_histogram_->topics.cap != 0 ? 2 * topic_histogram_->topics.cap : 1;
  topic_histogram_->counts.cap =
      topic_histogram_->counts.cap != 0 ? 2 * topic_histogram_->counts.cap : 1;
  if (topic_histogram_->topics.cap > static_cast<uint32_t>(NumTopics())) {
    topic_histogram_->topics.cap = NumTopics();
    topic_histogram_->counts.cap = NumTopics();
  }

  int32_t* new_topics = new int32_t[topic_histogram_->topics.cap];
  int64_t* new_counts = new int64_t[topic_histogram_->counts.cap];

  if (topic_histogram_->topics.array != NULL) {
    std::copy(static_cast<int32_t*>(topic_histogram_->topics.array),
              static_cast<int32_t*>(topic_histogram_->topics.array) +
              topic_histogram_->topics.len,
              new_topics);
    delete [] static_cast<int32_t*>(topic_histogram_->topics.array);
  }
  if (topic_histogram_->counts.array != NULL) {
    std::copy(static_cast<int64_t*>(topic_histogram_->counts.array),
              static_cast<int64_t*>(topic_histogram_->counts.array) +
              topic_histogram_->counts.len,
              new_counts);
    delete [] static_cast<int64_t*>(topic_histogram_->counts.array);
  }

  topic_histogram_->topics.array = new_topics;
  topic_histogram_->counts.array = new_counts;
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
