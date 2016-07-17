// Copyright (c) 2013 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_MODEL_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_MODEL_H_

#include <string>
#include <utility>
#include <vector>

#include "app/qzap/common/base/scoped_ptr.h"
#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/topic/base/common.h"
#include "app/qzap/text_analysis/topic/base/global_stats.h"
#include "app/qzap/text_analysis/topic/base/hyperparams.h"
#include "app/qzap/text_analysis/topic/base/word_stats.h"

namespace qzap {
namespace text_analysis {
namespace base {

// Model represents a shard or the whole Peacock model.  It includes
// the following parts:
//
//  - word stats, N(w,t)
//  - global stats, N(t)
//  - hyper-parameters
//
// When being used to represent a model shard, \sum_w N(w,t) < N(t).
//
// Model provides accessors to access variables like the number of
// topics, the vocabulary size, the part of vocabulary covered by
// N(w,t).
//
// Model also provides utilities like creating an empty model, loading
// a model, and printing a model in human-readable format.
//
class Model {
 public:
  // These constants define the file names in the model directory.
  static const std::string kGlobalStatsFilename;
  static const std::string kWordStatsFilename;
  static const std::string kHyperParamsFilename;

  explicit Model(int32_t num_topics);    // Initialize an empty model.
  explicit Model(const char* model_dir); // Load model from a directory.

  // Returns 0 when succeeded, -1 for IO error on word stats, -2 for
  // global stats, and -3 for hyper-parameters.
  int Load(const char* model_dir);
  // -4 for mkdir error.
  int Save(const char* model_dir) const;

  const Hyperparams& GetHyperparams() const { return *hyperparams_; }
  const GlobalStats& GetGlobalStats() const { return *global_stats_; }
  const WordStats& GetWordStats() const { return *word_stats_; }

  DenseTopicHistogram& GetGlobalTopicHistogram() {
    return global_stats_->GetTopicHistogram();
  }

  const DenseTopicHistogram& GetGlobalTopicHistogram() const {
    return global_stats_->GetTopicHistogram();
  }

  bool HasWord(int32_t word) const {
    return word_stats_->HasWord(word);
  }

  WordTopicHistogram& GetWordTopicHistogram(int32_t word) {
    return word_stats_->GetTopicHistogram(word);
  }

  const WordTopicHistogram& GetWordTopicHistogram(int32_t word) const {
    return word_stats_->GetTopicHistogram(word);
  }

  // Given word id, returns the pair consisting of max P(w|t) and the
  // corresponding topic id.
  std::pair<int32_t, double> GetMaxProbWordGivenTopic(int32_t word) const;

  double TopicPrior(int32_t i) const { return hyperparams_->TopicPrior(i); }
  double TopicPriorSum() const { return hyperparams_->TopicPriorSum(); }
  double WordPrior() const { return hyperparams_->WordPrior(); }
  double WordPriorSum() const { return hyperparams_->WordPriorSum(); }

  int32_t NumTopics() const { return global_stats_->NumTopics(); }
  int32_t NumWords()  const { return word_stats_->NumWords(); }
  int32_t VocabSize() const { return hyperparams_->VocabSize(); }

  // Get a row of the P(w|t) matrix, indexed by word
  void GetProbWordGivenTopic(int32_t word, DoubleVector* word_dist) const;

  // Get P(w|t) matrix by normalizing word_stats_ using global_stats_, indexed
  // by topic.
  void GetProbWordGivenTopic(SparseDoubleMatrix* topic_word_dist,
                             int32_t max_topic_words) const;

  void CalculateWordPriorOptimCount(
      std::vector<int32_t>* topic_len_count_ptr,
      std::vector<int32_t>* word_topic_count_ptr) const;

  void OptimTopicPrior(
      const std::vector<int32_t>& doc_len_count,
      const std::vector<std::vector<int32_t> > topic_doc_count,
      double shape, double scale, int32_t iteration_count) {
    hyperparams_->OptimTopicPrior(doc_len_count, topic_doc_count,
                                  shape, scale, iteration_count);
  }

  void OptimWordPrior(
      const std::vector<int32_t>& topic_len_count,
      const std::vector<int32_t>& word_topic_count,
      int32_t iteration_count) {
    hyperparams_->OptimWordPrior(topic_len_count, word_topic_count,
                                 iteration_count);
  }

  // Yes, we decided to expose all data members, as Model is simply an
  // aggregator, but not a wrapper.
  scoped_ptr<GlobalStats> global_stats_;
  scoped_ptr<WordStats> word_stats_;
  scoped_ptr<Hyperparams> hyperparams_;

 private:
  DECLARE_UNCOPYABLE(Model);
};

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_MODEL_H_
