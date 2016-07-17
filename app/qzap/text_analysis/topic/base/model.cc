// Copyright (c) 2012 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/base/model.h"

#include <float.h>  // For FLT_MIN
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/text_analysis/topic/base/common.h"

namespace qzap {
namespace text_analysis {
namespace base {

const std::string Model::kGlobalStatsFilename = "lda.global_stats";
const std::string Model::kWordStatsFilename = "lda.word_stats";
const std::string Model::kHyperParamsFilename = "lda.hyperparams";

Model::Model(int32_t num_topics) {
  global_stats_.reset(new GlobalStats);
  global_stats_->GetTopicHistogram().Resize(num_topics);
  word_stats_.reset(new WordStats(num_topics));
  hyperparams_.reset(new Hyperparams(0.1, num_topics, 0.01, 0));
}

Model::Model(const char* model_dir) {
  CHECK_EQ(0, Load(model_dir));
}

int Model::Load(const char* model_dir) {
  std::string dir(model_dir);
  if (dir[dir.size()-1] != '/') {
    dir.push_back('/');
  }

  global_stats_.reset(new GlobalStats);
  std::ifstream fin((dir + kGlobalStatsFilename).c_str());
  if (fin.fail() || !global_stats_->Load(&fin)) {
    LOG(ERROR) << "Model::Load failed loading: " << dir + kGlobalStatsFilename;
    return -1;
  }
  fin.close();

  word_stats_.reset(new WordStats(NumTopics()));
  fin.open((dir + kWordStatsFilename).c_str());
  if (fin.fail() || !word_stats_->Load(&fin)) {
    LOG(ERROR) << "Model::Load failed loading: " << dir + kWordStatsFilename;
    return -2;
  }
  fin.close();

  hyperparams_.reset(new Hyperparams(0.1, NumTopics(), 0.01, 0));
  fin.open((dir + kHyperParamsFilename).c_str());
  if (fin.fail() || !hyperparams_->Load(&fin)) {
    LOG(ERROR) <<"Model::Load failed loading: " << dir + kHyperParamsFilename;
    return -3;
  }
  fin.close();
  return 0;
}

int Model::Save(const char* model_dir) const {
  std::string dir(model_dir);
  if (dir[dir.size() - 1] != '/') {
    dir.push_back('/');
  }

  if (mkdir(model_dir, 0777) != 0) {
    LOG(ERROR) << "Failed to mkdir: " << model_dir;
    return -4;
  }

  std::ofstream fout((dir + kHyperParamsFilename).c_str());
  if (fout.fail() || !hyperparams_->Save(&fout)) {
    return -1;
  }
  fout.close();

  fout.open((dir + kWordStatsFilename).c_str());
  if (fout.fail() || !word_stats_->Save(&fout)) {
    return -2;
  }
  fout.close();

  fout.open((dir + kGlobalStatsFilename).c_str());
  if (fout.fail() || !global_stats_->Save(&fout)) {
    return -3;
  }
  fout.close();

  return 0;
}

void Model::GetProbWordGivenTopic(int32_t word,
                                  DoubleVector* word_dist) const {
  word_dist->resize(NumTopics());

  const DenseTopicHistogram& global_histogram = GetGlobalTopicHistogram();
  const double word_prior = WordPrior();
  const double word_prior_sum = WordPriorSum();

  // can be optimized!
  for (int32_t i = 0; i < NumTopics(); ++i) {
    CHECK(word_prior_sum + global_histogram[i] > FLT_MIN);
    (*word_dist)[i] = word_prior / (word_prior_sum + global_histogram[i]);
  }

  const WordTopicHistogram& topic_histogram = GetWordTopicHistogram(word);
  for (WordTopicHistogram::ConstIterator tit(topic_histogram);
       !tit.Done(); tit.Next()) {
    int32_t topic = tit.Topic();
    (*word_dist)[topic] = (word_prior + tit.Count()) /
        (word_prior_sum + global_histogram[topic]);
  }
}

void Model::GetProbWordGivenTopic(SparseDoubleMatrix* topic_word_dist,
                                  int32_t max_topic_words) const {
  topic_word_dist->resize(NumTopics());

  const DenseTopicHistogram& global_histogram = GetGlobalTopicHistogram();
  const double word_prior = WordPrior();
  const double word_prior_sum = WordPriorSum();

  for (WordStats::ConstIterator cit(word_stats_.get());
       !cit.Done(); cit.Next()) {
    int32_t word = cit.Word();
    const WordTopicHistogram& topic_histogram = GetWordTopicHistogram(word);
    for (WordTopicHistogram::ConstIterator tit(topic_histogram);
         !tit.Done(); tit.Next()) {
      int32_t topic = tit.Topic();
      (*topic_word_dist)[topic].push_back(
          std::make_pair(word,
                         (word_prior + tit.Count()) /
                         (word_prior_sum + global_histogram[topic])));
    }
  }

  // sort by words
  for (int32_t topic = 0; topic < NumTopics(); ++topic) {
    int32_t topk = std::min(
        max_topic_words,
        static_cast<int32_t>((*topic_word_dist)[topic].size()));

    std::partial_sort((*topic_word_dist)[topic].begin(),
              (*topic_word_dist)[topic].begin() + topk,
              (*topic_word_dist)[topic].end(),
              CompareByProb);
    (*topic_word_dist)[topic].erase((*topic_word_dist)[topic].begin() + topk,
              (*topic_word_dist)[topic].end());
    std::sort((*topic_word_dist)[topic].begin(),
              (*topic_word_dist)[topic].end(),
              CompareById);
  }
}

std::pair<int32_t, double> Model::GetMaxProbWordGivenTopic(int32_t word) const {
  std::pair<int32_t, double> max_prob_topic;
  max_prob_topic = std::make_pair(-1, -1);

  const DenseTopicHistogram& global_histogram = GetGlobalTopicHistogram();
  const double word_prior = WordPrior();
  const double word_prior_sum = WordPriorSum();

  for (int32_t i = 0; i < NumTopics(); ++i) {
    CHECK(word_prior_sum + global_histogram[i] > FLT_MIN);
    double prob = word_prior / (word_prior_sum + global_histogram[i]);
    if (prob > max_prob_topic.second) {
      max_prob_topic = std::make_pair(i, prob);
    }
  }

  const WordTopicHistogram& topic_histogram = GetWordTopicHistogram(word);
  for (WordTopicHistogram::ConstIterator tit(topic_histogram);
       !tit.Done(); tit.Next()) {
    int32_t topic = tit.Topic();
    double prob = (word_prior + tit.Count()) /
        (word_prior_sum + global_histogram[topic]);
    if (prob > max_prob_topic.second) {
      max_prob_topic = std::make_pair(topic, prob);
    }
  }

  return max_prob_topic;
}

void Model::CalculateWordPriorOptimCount(
    std::vector<int32_t>* topic_len_count_ptr,
    std::vector<int32_t>* word_topic_count_ptr) const {
  std::vector<int32_t>& topic_len_count = *topic_len_count_ptr;
  std::vector<int32_t>& word_topic_count = *word_topic_count_ptr;
  topic_len_count.clear();
  word_topic_count.clear();

  // ws = word_stats
  for (WordStats::ConstIterator ws_iter(word_stats_.get());
      !ws_iter.Done(); ws_iter.Next()) {
    // wth = word_topic_histogram
    const WordTopicHistogram& topic_histogram =
        ws_iter.GetTopicHistogram();
    for (WordTopicHistogram::ConstIterator wth_iter(topic_histogram);
        !wth_iter.Done(); wth_iter.Next()) {
      const int32_t count = wth_iter.Count();
      if (word_topic_count.size() <= static_cast<size_t>(count)) {
        word_topic_count.resize(count + 1);
      }
      ++word_topic_count[count];
    }
  }

  const DenseTopicHistogram& global_topic_histogram =
      global_stats_->GetTopicHistogram();
  for (int32_t i = 0; i < NumTopics(); ++i) {
    const int32_t count = global_topic_histogram[i];
    if (count == 0) { continue; }
    if (topic_len_count.size() <= static_cast<size_t>(count)) {
      topic_len_count.resize(count + 1);
    }
    ++topic_len_count[count];
  }
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
