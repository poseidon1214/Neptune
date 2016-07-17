// Copyright (c) 2011 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_WORD_STATS_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_WORD_STATS_H_

#include <fstream>
#include <string>
#include <tr1/unordered_map>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/topic/base/dense_topic_histogram.h"
#include "app/qzap/text_analysis/topic/base/ordered_sparse_histogram.h"
#include "app/qzap/text_analysis/topic/base/vocabulary.h"

namespace qzap {
namespace text_analysis {
namespace base {

typedef OrderedSparseHistogram WordTopicHistogram;

// WordStats stores topic histograms of words in the vocabulary
class WordStats {
 public:
  class ConstIterator {
   public:
    explicit ConstIterator(const WordStats* parent)
        : parent_(parent) {
      histogram_iterator_ = parent->word_topic_histograms_.begin();
    }

    bool Done() const {
      return histogram_iterator_ == parent_->word_topic_histograms_.end();
    }

    void Next() {
      CHECK(!Done());
      ++histogram_iterator_;
    }

    int32_t Word() const {
      CHECK(!Done());
      return histogram_iterator_->first;
    }

    const WordTopicHistogram& GetTopicHistogram() const {
      CHECK(!Done());
      return *(histogram_iterator_->second);
    }

    // if v != NULL and v->Size() > 0:
    //   fmt: word-str\ttopic:count topic:count ... \n
    // else:
    //   fmt: word-id\ttopic:count topic:count ... \n
    void AppendAsString(std::string* s, const Vocabulary* v) const {
      if (v != NULL && v->Size() > 0) {
        if (Word() >= v->Size() || Word() < 0) {
          StringAppendF(s, "UNKNOWN_%d\t", Word());
        } else {
          StringAppendF(s, "%s\t", v->Word(Word()).c_str());
        }
      } else {
        StringAppendF(s, "%d\t", Word());
      }
      GetTopicHistogram().AppendAsString(s);
      StringAppendF(s, "\n");
    }

   private:
    const WordStats* parent_;
    std::tr1::unordered_map<int32_t, WordTopicHistogram*>::const_iterator
        histogram_iterator_;
  };  // class ConstIterator

 public:
  explicit WordStats(int32_t num_topics)
      : zero_topic_histogram_(num_topics) {}
  ~WordStats() { Clear(); }

  bool Load(std::ifstream* fin, bool clear_old = true);

  bool LoadAndMerge(const std::vector<std::string>& filenames);

  bool Save(std::ofstream* fout) const;

  void ParseFrom(
      const std::tr1::unordered_map<int32_t, DenseTopicHistogram>& word_stats);

  // Indicates whether a word is in the model (word_topic_histograms_).
  bool HasWord(int32_t word) const {
    return word_topic_histograms_.find(word) != word_topic_histograms_.end();
  }

  // Prepare topic histogram allocation of a new word before
  // initializing histogram of the new word.
  void PreAllocTopicHistogram(int32_t word) {
    word_topic_histograms_[word] = new WordTopicHistogram(NumTopics());
  }

  // Returns the topic histogram for word.
  const WordTopicHistogram& GetTopicHistogram(int32_t word) const {
    CHECK(HasWord(word));
    return *(word_topic_histograms_.find(word)->second);
  }

  WordTopicHistogram& GetTopicHistogram(int32_t word) {
    CHECK(HasWord(word));
    return *(word_topic_histograms_[word]);
  }

  // Returns the number of words in the model
  // NOTE: !!!
  // WordStats.WordCount = Hyperparams.WordCount <= Vocabulary.Size
  int32_t NumWords() const {
    return static_cast<int32_t>(word_topic_histograms_.size());
  }
  int32_t NumTopics() const { return zero_topic_histogram_.NumTopics(); }
  int32_t NumSparseNodes() const;
  double SparseRatio() const;

  // This function is just for unittest.
  // If you have a 'large' model, pls use func ConstIterator::AppendAsString.
  //
  // if v != NULL and v->Size() > 0:
  //   fmt: word-str\ttopic:count topic:count ... \n
  // else:
  //   fmt: word-id\ttopic:count topic:count ... \n
  void AppendAsString(std::string* s, const Vocabulary* v) const;

 private:
  void Clear();

  friend class ConstIterator;

  // If users query a word for its topic histogram via GetTopicHistogram,
  // but this word does not appear in the training corpus,
  // GetTopicHistogram returns zero_topic_histogram.
  WordTopicHistogram zero_topic_histogram_;

  std::tr1::unordered_map<int32_t, WordTopicHistogram*>
      word_topic_histograms_;
};  // class WordStats

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_WORD_STATS_H_
