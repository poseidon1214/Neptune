// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/inference/explainer.h"

#include <algorithm>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "app/qzap/text_analysis/topic/base/common.h"
#include "app/qzap/text_analysis/topic/base/document.h"
#include "app/qzap/text_analysis/topic/base/model.h"
#include "app/qzap/text_analysis/topic/base/vocabulary.h"
#include "app/qzap/text_analysis/topic/inference/interpreter.h"

DEFINE_int32(peacock_model_max_topic_words, 20,
             "the maximum number of words for each topic.");
DEFINE_int32(peacock_topic_top_k, 10, "the maximum number of topics.");
DEFINE_int32(peacock_topic_word_top_k, 10,
             "the maximum number of topic words.");

namespace qzap {
namespace text_analysis {
namespace base {

Explainer::Explainer(const Model& model,
                     const Vocabulary& vocab,
                     const Interpreter& interpreter)
    : model_(model),
      vocab_(vocab),
      interpreter_(interpreter) {
  model_.GetProbWordGivenTopic(&topic_word_dist_,
                               FLAGS_peacock_model_max_topic_words);
}

void Explainer::Explain(const std::vector<std::string>& doc_words,
                        SparseDoubleVector* topic_dist,
                        SparseStringVector* word_dist) const {
  interpreter_.Interpret(doc_words, topic_dist);
  if (topic_dist->size() > FLAGS_peacock_topic_top_k) {
    topic_dist->erase(topic_dist->begin() + FLAGS_peacock_topic_top_k,
                      topic_dist->end());
  }
  GetTopicWords(*topic_dist, word_dist);
}

class TopicWord {
 public:
  TopicWord() {}
  TopicWord(const TopicWord& tw) {
    topic = std::make_pair(tw.topic.first, tw.topic.second);
    word_dist_citer = tw.word_dist_citer;
  }
  ~TopicWord() {}

  std::pair<int32_t/* topic */, double/* P(t|W) */> topic;
  SparseDoubleVector::const_iterator word_dist_citer;
  SparseDoubleVector::const_iterator word_dist_end;
};

struct TopicWordComparator :
    public std::binary_function<TopicWord*, TopicWord*, bool> {
  bool operator() (const TopicWord* lhs, const TopicWord* rhs) {
    return lhs->word_dist_citer->first > rhs->word_dist_citer->first;
  }
};

void Explainer::GetTopicWords(const SparseDoubleVector& topic_dist,
                              SparseStringVector* word_dist) const {
  std::priority_queue<TopicWord*, std::vector<TopicWord*>, TopicWordComparator>
      priority_queue;

  // initialize priority_queue
  int32_t topic_topk = std::min(static_cast<int32_t>(topic_dist.size()),
                                FLAGS_peacock_topic_top_k);
  for (int32_t i = 0; i < topic_topk; ++i) {
    TopicWord* topic_word = new TopicWord;
    topic_word->topic = topic_dist[i];
    topic_word->word_dist_citer =
        topic_word_dist_[topic_word->topic.first].begin();
    topic_word->word_dist_end =
        topic_word_dist_[topic_word->topic.first].end();

    priority_queue.push(topic_word);
  }

  // K-Way Merge
  word_dist->clear();
  std::pair<int32_t/* word */, double/* accumulated_prob */> current;
  current = std::make_pair(-1, 0);
  while (!priority_queue.empty()) {
    TopicWord* topic_word = priority_queue.top();
    priority_queue.pop();

    if (topic_word->word_dist_citer->first != current.first) {
      if (current.first != -1) {
        word_dist->push_back(
            std::make_pair(vocab_.Word(current.first), current.second));
      }
      current =
          std::make_pair(
              topic_word->word_dist_citer->first,
              topic_word->topic.second * topic_word->word_dist_citer->second);
    } else {
      current.second +=
          topic_word->topic.second * topic_word->word_dist_citer->second;
    }

    ++topic_word->word_dist_citer;
    if (topic_word->word_dist_citer != topic_word->word_dist_end) {
      priority_queue.push(topic_word);
    } else {
      delete topic_word;
    }
  }

  if (current.first != -1) {
    word_dist->push_back(
        std::make_pair(vocab_.Word(current.first), current.second));
  }

  int32_t topic_word_topk = std::min(static_cast<int32_t>(word_dist->size()),
                                     FLAGS_peacock_topic_word_top_k);
  // sort by words' probabilities
  std::partial_sort(word_dist->begin(),
                    word_dist->begin() + topic_word_topk,
                    word_dist->end(),
                    CompareByWordProb);
  if (word_dist->size() > topic_word_topk) {
    word_dist->erase(word_dist->begin() + topic_word_topk, word_dist->end());
  }
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

