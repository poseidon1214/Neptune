// Copyright (c) 2013 Tencent Inc.
// Author: YAN Hao (charlieyan@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_SPARSELDA_HILL_CLIMBER_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_SPARSELDA_HILL_CLIMBER_H_

#include <stdint.h>
#include <tr1/unordered_map>
#include <utility>
#include <vector>

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/text_analysis/topic/base/smoothed_model_cache.h"
#include "app/qzap/text_analysis/topic/inference/interpreter.h"

namespace qzap {
namespace text_analysis {
namespace base {

class Random;
class Document;
class Model;
class OrderedSparseHistogram;
class Vocabulary;

// SparseLDAHillClimber is designed for large scale LDA model, say 300k words
// and 1M topics.  To achieve acceptable efficiency, this algorithm takes
// advantage of both SparseLDA and hill-climbing.  And more, topic distributions
// of frequent words are pre-computed and cached.  We do not cache the whole
// vocabulary for that may cost too much memory.
class SparseLDAHillClimber : public Interpreter {
 public:
  SparseLDAHillClimber(const Model& model,
                       const Vocabulary& vocab,
                       int64_t cache_size_mb,
                       int32_t total_iterations)
      : Interpreter(model, vocab, total_iterations) {
    InitializeCache(cache_size_mb);
  }

  virtual ~SparseLDAHillClimber() {}

  // Interpret use a trained model to interpret topics embedded in the given
  // document doc_tokens. This function returns topics sorted by their
  // probabilities.
  virtual void Interpret(
      const std::vector<std::string/*word*/>& doc_tokens,
      std::vector<std::pair<int32_t/*topic*/, double/*P_t|W*/> >* topic_dist)
      const;

 protected:
  // Words in vocab.GetOrderedWordList() are sorted by frequency in descending
  // order. While initializing cache, the words' topic distributions are cached
  // word by word in the same order, till memory size meets the limit.
  void InitializeCache(int64_t cache_size_mb/* in MB */);

  // Calculate s(z,w) = \alpha_z * p(w|z), and cache the max{s(z,w)} for all
  // words
  void CacheWordTopicPriorMax();

  // InterpretOneChain performs one markov chain.
  // Returns the topic distribution of the last iteration (total_iterations).
  void InterpretOneTrail(
    const std::vector<std::string>& doc_tokens,
    std::vector<std::pair<int32_t, double> >* topic_dist,
    base::Random* random) const;

  int32_t SampleNewTopic(Document* doc, int32_t word) const;

  // Converting the topic histogram (counting distribution) to
  // topic distribution (probability distribution)
  virtual void UnifyDistribution(
      const OrderedSparseHistogram& topic_hist,
      std::vector<std::pair<int32_t/*topic*/, double/*P_t|d*/> >* topic_dist)
      const;

  scoped_ptr<SmoothedModelCache> smoothed_model_cache_;

  std::tr1::unordered_map<int32_t/*word*/,
      std::pair<int32_t/*topic*/, double/*alpha_z * p(w|z)*/> >
          smoothing_only_max_;
};  // class SparseLDAHillClimber

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_INFERENCE_SPARSELDA_HILL_CLIMBER_H_
