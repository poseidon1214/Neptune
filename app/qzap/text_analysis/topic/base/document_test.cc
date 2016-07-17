// Copyright 2010 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/base/document.h"

#include <string>

#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/topic/base/cgo_types.h"
#include "app/qzap/text_analysis/topic/base/common.h"
#include "app/qzap/text_analysis/topic/base/model.h"
#include "app/qzap/text_analysis/topic/base/random.h"

namespace qzap {
namespace text_analysis {
namespace base {

const std::string kModelDir = "testdata/model-standard";
const std::string kVocabFile = "testdata/document_test.vocab";
const int32_t kRNGSeed = 0;

TEST(DocumentTest, Ctor) {
  // usage 1: the document should be allocated at construction time
  Document cpp_doc1(0);

  EXPECT_EQ(0, cpp_doc1.NumTopics());
  EXPECT_EQ(cpp_doc1.NumTopics(), cpp_doc1.GetTopicHistogram()->NumTopics());

  // usage 2: the document is already there in Go/C format
  ::Document* c_doc = ::NewDocument();
  Document cpp_doc2(c_doc, 5);
  EXPECT_EQ(5, cpp_doc2.NumTopics());
  EXPECT_EQ(cpp_doc2.NumTopics(), cpp_doc2.GetTopicHistogram()->NumTopics());
  EXPECT_EQ(&c_doc->words, cpp_doc2.GetWords());
  EXPECT_NE(&c_doc->hist, cpp_doc2.GetTopicHistogram()->CType());
  // Note: you must free histogram outside
  ::OrderedSparseHistogram* hist = cpp_doc2.GetTopicHistogram()->CType();
  ::DeleteOrderedSparseHistogram(hist);

  ::DeleteDocument(c_doc);
}

TEST(DocumentTest, ParseFromTokens) {
  Model model(kModelDir.c_str());
  Vocabulary vocab(kVocabFile.c_str());

  base::MTRandom rng;
  rng.SeedRNG(kRNGSeed);

  // empty
  std::vector<std::string> doc_tokens;
  Document doc1(3);
  doc1.ParseFromTokens(doc_tokens, vocab, model, &rng);
  EXPECT_EQ(0, doc1.NumWords());
  EXPECT_TRUE(doc1.GetConstTopicHistogram()->IsOrdered());

  // regular
  doc_tokens.push_back("apple");
  doc_tokens.push_back("orange");
  doc_tokens.push_back("banana");
  Document doc2(3);
  doc2.ParseFromTokens(doc_tokens, vocab, model, &rng);
  EXPECT_EQ(3, doc2.NumWords());
  EXPECT_TRUE(doc2.GetConstTopicHistogram()->IsOrdered());

  // out of vocabulary
  doc_tokens.clear();
  doc_tokens.push_back("apple");
  doc_tokens.push_back("orange");
  doc_tokens.push_back("banana");
  doc_tokens.push_back("null");
  Document doc3(3);
  doc3.ParseFromTokens(doc_tokens, vocab, model, &rng);
  EXPECT_EQ(3, doc3.NumWords());
  EXPECT_TRUE(doc3.GetConstTopicHistogram()->IsOrdered());

  // repeated words
  doc_tokens.clear();
  doc_tokens.push_back("apple");
  doc_tokens.push_back("apple");
  doc_tokens.push_back("orange");
  doc_tokens.push_back("dog");
  doc_tokens.push_back("dog");
  doc_tokens.push_back("dog");
  Document doc4(3);
  doc4.ParseFromTokens(doc_tokens, vocab, model, &rng);
  EXPECT_EQ(6, doc4.NumWords());
  EXPECT_TRUE(doc4.GetConstTopicHistogram()->IsOrdered());
}

TEST(DocumentTest, CalculateTopicHistogram) {
  ::Document* c_doc = ::NewDocument();

  ::Slice* words = &c_doc->words;
  words->array = new ::Word[4];
  words->len = 4;
  words->cap = 4;
  reinterpret_cast<Word*>(words->array)[0].id = 0;
  reinterpret_cast<Word*>(words->array)[0].topic = 0;
  reinterpret_cast<Word*>(words->array)[1].id = 0;
  reinterpret_cast<Word*>(words->array)[1].topic = 1;
  reinterpret_cast<Word*>(words->array)[2].id = 1;
  reinterpret_cast<Word*>(words->array)[2].topic = 0;
  reinterpret_cast<Word*>(words->array)[3].id = 1;
  reinterpret_cast<Word*>(words->array)[3].topic = 3;

  Document cpp_doc(c_doc, 5);
  cpp_doc.CalculateTopicHistogram();

  EXPECT_EQ(cpp_doc.TopicHistogramValue(0), 2);
  EXPECT_EQ(cpp_doc.TopicHistogramValue(1), 1);
  EXPECT_EQ(cpp_doc.TopicHistogramValue(2), 0);
  EXPECT_EQ(cpp_doc.TopicHistogramValue(3), 1);
  EXPECT_EQ(cpp_doc.TopicHistogramValue(4), 0);

  ::DeleteDocument(c_doc);
}

TEST(DocumentTest, Iterator) {
  ::Document* c_doc = ::NewDocument();

  ::Slice* words = &c_doc->words;
  words->array = new ::Word[4];
  words->len = 4;
  words->cap = 4;
  reinterpret_cast<Word*>(words->array)[0].id = 0;
  reinterpret_cast<Word*>(words->array)[0].topic = 0;
  reinterpret_cast<Word*>(words->array)[1].id = 0;
  reinterpret_cast<Word*>(words->array)[1].topic = 1;
  reinterpret_cast<Word*>(words->array)[2].id = 1;
  reinterpret_cast<Word*>(words->array)[2].topic = 0;
  reinterpret_cast<Word*>(words->array)[3].id = 1;
  reinterpret_cast<Word*>(words->array)[3].topic = 3;

  Document cpp_doc(c_doc, 5);
  cpp_doc.CalculateTopicHistogram();

  Document::Iterator it1(&cpp_doc);  // word 0
  it1.Next();  // word 0
  it1.Next();  // word 1
  cpp_doc.DecrementTopicHistogram(it1.Topic(), 1);
  it1.SetTopic(4);
  cpp_doc.IncrementTopicHistogram(it1.Topic(), 1);

  Document::ConstIterator it2(cpp_doc);
  EXPECT_EQ(it2.Word(), 0);
  EXPECT_EQ(it2.Topic(), 0);
  it2.Next();
  EXPECT_EQ(it2.Word(), 0);
  EXPECT_EQ(it2.Topic(), 1);
  it2.Next();
  EXPECT_EQ(it2.Word(), 1);
  EXPECT_EQ(it2.Topic(), 4);
  it2.Next();
  EXPECT_EQ(it2.Word(), 1);
  EXPECT_EQ(it2.Topic(), 3);
  it2.Next();
  EXPECT_EQ(it2.Done(), true);

  EXPECT_EQ(cpp_doc.TopicHistogramValue(0), 1);
  EXPECT_EQ(cpp_doc.TopicHistogramValue(1), 1);
  EXPECT_EQ(cpp_doc.TopicHistogramValue(2), 0);
  EXPECT_EQ(cpp_doc.TopicHistogramValue(3), 1);
  EXPECT_EQ(cpp_doc.TopicHistogramValue(4), 1);

  ::DeleteDocument(c_doc);
}

TEST(DocumentTest, String) {
  Model model(kModelDir.c_str());
  Vocabulary vocab(kVocabFile.c_str());

  base::MTRandom rng;
  rng.SeedRNG(kRNGSeed);

  // empty
  std::vector<std::string> doc_tokens;
  Document doc1(3);
  doc1.ParseFromTokens(doc_tokens, vocab, model, &rng);
  EXPECT_EQ(0, doc1.NumWords());
  EXPECT_EQ("", doc1.String(vocab));

  // regular
  doc_tokens.push_back("apple");
  doc_tokens.push_back("orange");
  doc_tokens.push_back("banana");
  Document doc2(3);
  doc2.ParseFromTokens(doc_tokens, vocab, model, &rng);
  EXPECT_EQ(3, doc2.NumWords());
  EXPECT_EQ("apple\torange\tbanana\t", doc2.String(vocab));

  // out of vocabulary
  doc_tokens.clear();
  doc_tokens.push_back("apple");
  doc_tokens.push_back("orange");
  doc_tokens.push_back("banana");
  doc_tokens.push_back("null");
  Document doc3(3);
  doc3.ParseFromTokens(doc_tokens, vocab, model, &rng);
  EXPECT_EQ(3, doc3.NumWords());
  EXPECT_EQ("apple\torange\tbanana\t", doc3.String(vocab));

  // repeated words
  doc_tokens.clear();
  doc_tokens.push_back("apple");
  doc_tokens.push_back("apple");
  doc_tokens.push_back("orange");
  doc_tokens.push_back("dog");
  doc_tokens.push_back("dog");
  doc_tokens.push_back("dog");
  Document doc4(3);
  doc4.ParseFromTokens(doc_tokens, vocab, model, &rng);
  EXPECT_EQ(6, doc4.NumWords());
  EXPECT_EQ("apple\tapple\torange\tdog\tdog\tdog\t", doc4.String(vocab));
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

