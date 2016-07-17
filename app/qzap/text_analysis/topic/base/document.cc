// Copyright 2010 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/base/document.h"

#include <string>
#include <vector>

#include "app/qzap/text_analysis/topic/base/common.h"
#include "app/qzap/text_analysis/topic/base/model.h"
#include "app/qzap/text_analysis/topic/base/random.h"

namespace qzap {
namespace text_analysis {
namespace base {

Document::Document(int32_t num_topics)
  : words_(::NewSlice()),
    doc_topic_histogram_(num_topics),
    own_memory_(true) {}

Document::Document(const ::Document* doc, int32_t num_topics)
  : words_(&(const_cast< ::Document*>(doc)->words)),
    doc_topic_histogram_(&(const_cast< ::Document*>(doc)->hist), num_topics),
    own_memory_(false) {}

Document::~Document() {
  if (own_memory_) {
    ::DeleteWordSlice(words_);
  }
  words_ = NULL;
}

void Document::ParseFromTokens(const std::vector<std::string>& doc_tokens,
                               const Vocabulary& vocab,
                               const Model& model,
                               Random* random) {
  if (own_memory_) {
    if (words_ != NULL && words_->len > 0) {
      ::DeleteWordSlice(words_);
      words_ = ::NewSlice();
    }
  }

  std::vector<int32_t> word_ids;
  for (size_t i = 0; i < doc_tokens.size(); ++i) {
    int32_t word_id = vocab.WordIndex(doc_tokens[i]);
    if (word_id >= 0 && model.HasWord(word_id)) {
      word_ids.push_back(word_id);
    }
  }

  if (word_ids.size() == 0) { return; }

  words_->array = new ::Word[word_ids.size()];
  words_->len = word_ids.size();
  words_->cap = word_ids.size();

  for (size_t i = 0; i < word_ids.size(); ++i) {
    reinterpret_cast< ::Word*>(words_->array)[i].id = word_ids[i];
    reinterpret_cast< ::Word*>(words_->array)[i].topic =
         random->RandInt32(NumTopics());
    doc_topic_histogram_.IncrementTopic(
        reinterpret_cast< ::Word*>(words_->array)[i].topic, 1);

  }
}

void Document::CalculateTopicHistogram() {
  for (Iterator it(this); !it.Done(); it.Next()) {
    doc_topic_histogram_.IncrementTopic(it.Topic(), 1);
  }
}

int32_t Document::Length() const {
  int32_t len = 0;
  for (DocumentTopicHistogram::ConstIterator citer(doc_topic_histogram_);
      !citer.Done(); citer.Next()) {
    len += citer.Count();
  }
  return len;
}

std::string Document::String(const Vocabulary& vocab) const {
  std::string doc_str;
  if (words_ != NULL) {
    for (uint32_t i = 0; i < words_->len; ++i) {
      doc_str += vocab.Word(reinterpret_cast< ::Word*>(words_->array)[i].id)
          + "\t";
    }
  }
  return doc_str;
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
