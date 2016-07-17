// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include <fstream>
#include<numeric>

#include "app/qzap/text_analysis/topic/base/hyperparams.h"

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/text_analysis/topic/kv_records/kv_records.h"

namespace qzap {
namespace text_analysis {
namespace base {

using std::string;

Hyperparams::Hyperparams(double topic_prior,
                         int32_t num_topics,
                         double word_prior,
                         int32_t vocab_size) {
  Set(topic_prior, num_topics, word_prior, vocab_size);
}

Hyperparams::Hyperparams(const std::vector<double>& topic_prior,
                         double word_prior,
                         int32_t vocab_size) {
  topic_prior_ = topic_prior;
  topic_prior_sum_ = accumulate(topic_prior_.begin(),
                                topic_prior_.end(), 0.0);
  word_prior_ = word_prior;
  word_prior_sum_ = word_prior * vocab_size;
  vocab_size_ = vocab_size;
}

Hyperparams::Hyperparams(const std::string& filename) {
  std::ifstream fin(filename.c_str());
  CHECK(!fin.fail());
  CHECK(Load(&fin));
  fin.close();
}

void Hyperparams::Set(double topic_prior,
                      int32_t num_topics,
                      double word_prior,
                      int32_t vocab_size) {
  topic_prior_.clear();
  topic_prior_.resize(num_topics, topic_prior);
  topic_prior_sum_ = topic_prior * num_topics;
  word_prior_ = word_prior;
  word_prior_sum_ = word_prior * vocab_size;
  vocab_size_ = vocab_size;
}

void Hyperparams::ParseFrom(const HyperparamsPB& pb) {
  topic_prior_.resize(pb.topic_prior_size());
  topic_prior_sum_ = 0;
  for (int32_t i = 0; i < pb.topic_prior_size(); ++i) {
    topic_prior_[i] = pb.topic_prior(i);
    topic_prior_sum_ += topic_prior_[i];
  }

  word_prior_ = pb.word_prior();
  vocab_size_ = pb.vocab_size();
  word_prior_sum_ = word_prior_ * vocab_size_;
}

void Hyperparams::SerializeTo(HyperparamsPB* pb) const {
  pb->Clear();
  for (size_t i = 0; i < topic_prior_.size(); ++i) {
    pb->add_topic_prior(topic_prior_[i]);
  }
  pb->set_word_prior(word_prior_);
  pb->set_vocab_size(vocab_size_);
}

// local file or xfs file
bool Hyperparams::Load(std::ifstream* fin) {
  KVRecordReader r(fin);
  string key, value;
  if (r.Read(&key, &value) != 0) {
    return false;
  }

  HyperparamsPB pb;
  if (!pb.ParseFromString(value)) {
    LOG(ERROR) << "Message::ParseFromString failed";
    return false;
  }
  ParseFrom(pb);
  return true;
}

bool Hyperparams::Save(std::ofstream* fout) const {
  KVRecordWriter w(fout);
  string key(" "), value;
  HyperparamsPB hyperparams_pb;
  SerializeTo(&hyperparams_pb);
  hyperparams_pb.SerializeToString(&value);
  if (w.Write(key, value) != 0) {
    return false;
  }
  return true;
}

void Hyperparams::OptimTopicPrior(
    const std::vector<int32_t>& doc_len_count,
    const std::vector<std::vector<int32_t> > topic_doc_count,
    double shape, double scale, int32_t iteration_count) {
  CHECK(static_cast<int32_t>(topic_doc_count.size()) == NumTopics());

  double diff_digamma;
  for (int32_t it = 0; it < iteration_count; ++it) {
    double denom = 0;
    diff_digamma = 0;
    for (size_t j = 1; j < doc_len_count.size(); ++j) {
      diff_digamma += 1.0 / (j - 1 + topic_prior_sum_);
      denom += doc_len_count[j] * diff_digamma;
    }
    denom -= 1.0/scale;

    topic_prior_sum_ = 0;
    for (size_t k = 0; k < topic_doc_count.size(); ++k) {
      double num = 0;
      diff_digamma = 0;
      for (size_t j = 1; j < topic_doc_count[k].size(); ++j) {
        diff_digamma += 1.0 / (j - 1 + topic_prior_[k]);
        num += topic_doc_count[k][j] *  diff_digamma;
      }
      topic_prior_[k] = (topic_prior_[k] * num + shape) / denom;
      topic_prior_sum_ += topic_prior_[k];
    }
  }
}

void Hyperparams::OptimWordPrior(
    const std::vector<int32_t>& topic_len_count,
    const std::vector<int32_t>& word_topic_count,
    int32_t iteration_count) {
  double diff_digamma;
  for (int32_t it = 0; it < iteration_count; ++it) {
    double num = 0;
    diff_digamma = 0;
    for (size_t j = 1; j < word_topic_count.size(); ++j) {
      diff_digamma += 1.0 / (j - 1 + word_prior_);
      num += diff_digamma * word_topic_count[j];
    }

    double denom = 0;
    diff_digamma = 0;
    for (size_t j = 1; j < topic_len_count.size(); ++j) {
      diff_digamma += 1.0 / (j - 1 + word_prior_sum_);
      denom += diff_digamma * topic_len_count[j];
    }
    word_prior_sum_ = word_prior_ * num / denom;
    word_prior_ = word_prior_sum_ / vocab_size_;
  }
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
