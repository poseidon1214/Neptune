// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/base/smoothed_model_cache.h"

#include <queue>
#include <utility>
#include <vector>

#include "app/qzap/text_analysis/topic/base/common.h"
#include "app/qzap/text_analysis/topic/base/model.h"
#include "app/qzap/text_analysis/topic/base/ordered_sparse_histogram.h"
#include "app/qzap/text_analysis/topic/base/word_stats.h"

namespace qzap {
namespace text_analysis {
namespace base {

class MoreFrequent {
 public:
  bool operator() (const std::pair<int32_t, int64_t>& arg1,
                   const std::pair<int32_t, int64_t>& arg2) {
    return arg1.second > arg2.second;
  }
};

typedef std::priority_queue<std::pair<int32_t, int64_t>,
        std::vector<std::pair<int32_t, int64_t> >,
        MoreFrequent> WordPriorityQueue;

// Words frenquency in word_stats are calculated, and returns num_words
// most frenquent ones.
void GetWordPriorityQueue(const WordStats& word_stats,
                          const int32_t num_words,
                          WordPriorityQueue* word_queue) {
  for (WordStats::ConstIterator citer(&word_stats);
       !citer.Done(); citer.Next()) {
    int32_t word = citer.Word();
    const WordTopicHistogram& word_topic_hist =
        word_stats.GetTopicHistogram(word);
    int32_t topic_count = 0;
    for (WordTopicHistogram::ConstIterator tit(word_topic_hist);
         !tit.Done(); tit.Next()) {
      topic_count += tit.Count();
    }

    word_queue->push(std::make_pair(word, topic_count));
    if (word_queue->size() > static_cast<uint32_t>(num_words)) {
      word_queue->pop();
    }
  }
}

void SmoothedModelCache::Compute(const Model& model, int64_t cache_size_mb) {
  smoothing_only_matrix_.clear();

  int32_t num_topics = model.NumTopics();
  int32_t num_words_cached = static_cast<int32_t>(
      1024 * 1024 * cache_size_mb / (sizeof(double) * num_topics));
  const WordStats& word_stats = model.GetWordStats();

  if (num_words_cached >= model.NumWords()) {
    for (WordStats::ConstIterator citer(&word_stats);
         !citer.Done(); citer.Next()) {
      int32_t word = citer.Word();

      // Calculate the p(w|z), then cache it in smoothing_only_matrix_.
      smoothing_only_matrix_[word].resize(num_topics);
      model.GetProbWordGivenTopic(word, &smoothing_only_matrix_[word]);
    }
  } else {
    WordPriorityQueue word_queue;
    GetWordPriorityQueue(word_stats, num_words_cached, &word_queue);

    while (!word_queue.empty()) {
      std::pair<int32_t, int64_t> word_pair = word_queue.top();
      word_queue.pop();

      // Calculate the p(w|z), then cache it in smoothing_only_matrix_.
      smoothing_only_matrix_[word_pair.first].resize(num_topics);
      model.GetProbWordGivenTopic(word_pair.first,
                                  &smoothing_only_matrix_[word_pair.first]);
    }
  }
}

const DoubleVector* SmoothedModelCache::GetProbWordGivenTopic(
    int32_t word) const {
  DoubleMatrix::const_iterator iter = smoothing_only_matrix_.find(word);
  if (iter != smoothing_only_matrix_.end()) {
    return &iter->second;
  }

  return NULL;
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

