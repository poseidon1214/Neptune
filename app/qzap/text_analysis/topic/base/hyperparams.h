// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_HYPERPARAMS_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_HYPERPARAMS_H_

#include <fstream>
#include <string>
#include <vector>

#include "thirdparty/glog/logging.h"

#include "app/qzap/text_analysis/topic/base/lda.pb.h"

namespace qzap {
namespace text_analysis {
namespace base {

class Hyperparams {
 public:
  Hyperparams(double topic_prior, int32_t num_topics,
              double word_prior, int32_t vocab_size);

  Hyperparams(const std::vector<double>& topic_prior,
              double word_prior, int32_t vocab_size);

  explicit Hyperparams(const std::string& filename);

  void Set(double topic_prior, int32_t num_topics,
           double word_prior, int32_t vocab_size);

  void ParseFrom(const HyperparamsPB& pb);

  void SerializeTo(HyperparamsPB* pb) const;

  // local file or xfs file
  bool Load(std::ifstream* fin);
  bool Save(std::ofstream* fout) const;

  double TopicPrior(int32_t i) const { return topic_prior_[i]; }
  double TopicPriorSum() const { return topic_prior_sum_; }
  double WordPrior() const { return word_prior_; }
  double WordPriorSum() const { return word_prior_sum_; }

  int32_t NumTopics() const {
    return static_cast<int32_t>(topic_prior_.size());
  }

  int32_t VocabSize() const {
    return vocab_size_;
  }

  // asymmetric Dirichlet prior
  void OptimTopicPrior(
      const std::vector<int32_t>& doc_len_count,
      const std::vector<std::vector<int32_t> > topic_doc_count,
      double shape, double scale, int32_t iteration_count);

  // symmetric Dirichlet prior
  void OptimWordPrior(
      const std::vector<int32_t>& topic_len_count,
      const std::vector<int32_t>& word_topic_count,
      int32_t iteration_count);

 private:
  std::vector<double> topic_prior_;
  double topic_prior_sum_;
  double word_prior_;
  double word_prior_sum_;
  int32_t vocab_size_;
};  // class Hyperparams

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_HYPERPARAMS_H_
