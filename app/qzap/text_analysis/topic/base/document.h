// Copyright 2010 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_DOCUMENT_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_DOCUMENT_H_

#include "thirdparty/glog/logging.h"
#include "app/qzap/text_analysis/topic/base/cgo_types.h"
#include "app/qzap/text_analysis/topic/base/common.h"
#include "app/qzap/text_analysis/topic/base/ordered_sparse_histogram.h"
#include "app/qzap/text_analysis/topic/base/vocabulary.h"

namespace qzap {
namespace text_analysis {
namespace base {

typedef OrderedSparseHistogram DocumentTopicHistogram;

class Model;

class Document {
 public:
  // A const_iterator over all of the word occurrences in a document.
  class ConstIterator {
   public:
    // ctor
    explicit ConstIterator(const Document& parent)
        : parent_(parent),
          word_index_(0) {}

    // Returns true if we are done iterating.
    bool Done() const {
      CHECK_GE(parent_.words_->len, word_index_);
      // Have we advanced beyond the last word?
      return parent_.words_->array == NULL ||
          word_index_ == parent_.words_->len;
    }

    // We iterate over all the occurrences of each word. If we
    // have finished with the current word, we advance to the 0th
    // ocurrence of the next word that has occurrences.
    void Next() {
      CHECK(!Done());
      ++word_index_;
    }

    // Returns the word of the current occurrence.
    int32_t Word() const {
      CHECK(!Done());
      return static_cast< ::Word*>(parent_.words_->array)[word_index_].id;
    }

    // Returns the topic of the current occurrence.
    int32_t Topic() const {
      CHECK(!Done());
      return static_cast< ::Word*>(parent_.words_->array)[word_index_].topic;
    }

   protected:
    const Document& parent_;
    uint32_t word_index_;
  };  // class Iterator

  // An iterator over all of the word occurrences in a document.
  class Iterator : public ConstIterator {
   public:
    // ctor
    explicit Iterator(Document* parent) : ConstIterator(*parent) {}

    // NOTE: topic histogram needs to be updated by user.
    void SetTopic(int32_t new_topic) {
      CHECK(!Done());
      static_cast< ::Word*>(parent_.words_->array)[word_index_].topic =
          new_topic;
    }
  };  // class Iterator

 public:
  // There are two supposed usages of Document: the document should be
  // allocated at construction time, or the document is already there in Go/C
  // format.  The former happens in inference, when usually construct the
  // document by ParseFromText(), and the latter happens in training.
  // We define constructors for the cases respectively
  explicit Document(int32_t num_topics);
  Document(const ::Document* doc, int32_t num_topics);
  ~Document();

  // ParseFromTokens parses a text document from doc_tokens.
  // Note: only the words in vocab will be considered into document
  void ParseFromTokens(const std::vector<std::string>& doc_tokens,
                       const Vocabulary& vocab,
                       const Model& model,
                       Random* random);

  // calculates current document's topic histogram based on words
  void CalculateTopicHistogram();

  int32_t NumWords() const { return words_->len; }
  int32_t Length() const;
  int32_t NumTopics() const { return doc_topic_histogram_.NumTopics(); }

  ::Slice* GetWords() const { return words_; }

  DocumentTopicHistogram* GetTopicHistogram() {
    return &doc_topic_histogram_;
  }

  const DocumentTopicHistogram* GetConstTopicHistogram() const {
    return &doc_topic_histogram_;
  }

  void IncrementTopicHistogram(int32_t topic, int32_t count) {
    doc_topic_histogram_.IncrementTopic(topic, count);
  }

  void DecrementTopicHistogram(int32_t topic, int32_t count) {
    doc_topic_histogram_.DecrementTopic(topic, count);
  }

  int64_t TopicHistogramValue(int32_t topic) const {
    return doc_topic_histogram_.Count(topic);
  }

  std::string String(const Vocabulary& vocab) const;

 private:
  friend class Iterator;

  ::Slice* words_;   // word occurrences and topic assignments.
  DocumentTopicHistogram doc_topic_histogram_;

  // whether class Document should free words_
  // true: the words_ should be allocated and deallocated inside
  // false: the document only shares the words_ with Document in C-type
  bool own_memory_;
};  // class Document

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_DOCUMENT_H_
