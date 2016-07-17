// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/inference/explainer.h"

#include "thirdparty/gtest/gtest.h"

#include "app/qzap/text_analysis/topic/base/common.h"
#include "app/qzap/text_analysis/topic/base/document.h"
#include "app/qzap/text_analysis/topic/base/model.h"
#include "app/qzap/text_analysis/topic/base/vocabulary.h"
#include "app/qzap/text_analysis/topic/inference/multi_chains_gibbs_sampler.h"
#include "app/qzap/text_analysis/topic/inference/multi_trials_hill_climber.h"
#include "app/qzap/text_analysis/topic/inference/sparselda_gibbs_sampler.h"
#include "app/qzap/text_analysis/topic/inference/sparselda_hill_climber.h"

namespace qzap {
namespace text_analysis {
namespace base {

static const char kModelDir[] = "testdata/model-standard";
static const char kVocabFile[] = "testdata/document_test.vocab";
static const double kEpsilon = 1E-6;

class ExplainerTest : public ::testing::Test {
 public:
  ExplainerTest() : model_(0) {}

  virtual ~ExplainerTest() {}

  virtual void SetUp() {
    CHECK_EQ(0, model_.Load(kModelDir));
    CHECK(vocab_.Load(kVocabFile));
  }

  void TestExplain() {
    explainer_.reset(new Explainer(model_, vocab_, *interpreter_));
    std::vector<std::string> doc_words;
    std::vector<std::pair<std::string, double> > word_dist;
    std::vector<std::pair<int, double> > topic_dist;

    size_t i = 0;
    explainer_->Explain(doc_words, &topic_dist, &word_dist);
    EXPECT_EQ(0u, word_dist.size());

    i = 1;
    doc_words.push_back("apple");
    doc_words.push_back("orange");
    doc_words.push_back("banana");
    explainer_->Explain(doc_words, &topic_dist, &word_dist);
    EXPECT_EQ(3u, word_dist.size());
    EXPECT_EQ("apple", word_dist[0].first);
    EXPECT_EQ("orange", word_dist[1].first);
    EXPECT_EQ("banana", word_dist[2].first);
    EXPECT_NEAR(0.32238, word_dist[0].second, kEpsilon);
    EXPECT_NEAR(0.32238, word_dist[1].second, kEpsilon);
    EXPECT_NEAR(0.32238, word_dist[2].second, kEpsilon);

    i = 2;
    doc_words.clear();
    doc_words.push_back("apple");
    doc_words.push_back("orange");
    doc_words.push_back("dog");
    doc_words.push_back("haha");
    explainer_->Explain(doc_words, &topic_dist, &word_dist);
    EXPECT_EQ(6u, word_dist.size());
    EXPECT_EQ("orange", word_dist[0].first);
    EXPECT_EQ("apple", word_dist[1].first);
    EXPECT_EQ("banana", word_dist[2].first);
    EXPECT_EQ("dog", word_dist[3].first);
    EXPECT_EQ("cat", word_dist[4].first);
    EXPECT_EQ("tiger", word_dist[5].first);
    EXPECT_NEAR(0.21838663, word_dist[0].second, kEpsilon);
    EXPECT_NEAR(0.21838663, word_dist[1].second, kEpsilon);
    EXPECT_NEAR(0.21838663, word_dist[2].second, kEpsilon);
    EXPECT_NEAR(0.11439300, word_dist[3].second, kEpsilon);
    EXPECT_NEAR(0.11439300, word_dist[4].second, kEpsilon);
    EXPECT_NEAR(0.11439300, word_dist[5].second, kEpsilon);
  }

  void TestExplainMultiChain() {
    explainer_.reset(new Explainer(model_, vocab_, *interpreter_));
    std::vector<std::string> doc_words;
    std::vector<std::pair<std::string, double> > word_dist;
    std::vector<std::pair<int, double> > topic_dist;

    size_t i = 0;
    explainer_->Explain(doc_words, &topic_dist, &word_dist);
    EXPECT_EQ(0u, word_dist.size());

    i = 1;
    doc_words.push_back("apple");
    doc_words.push_back("orange");
    doc_words.push_back("banana");
    explainer_->Explain(doc_words, &topic_dist, &word_dist);
    EXPECT_EQ(3u, word_dist.size());
    EXPECT_EQ("apple", word_dist[0].first);
    EXPECT_EQ("orange", word_dist[1].first);
    EXPECT_EQ("banana", word_dist[2].first);
    EXPECT_NEAR(0.32238, word_dist[0].second, kEpsilon);
    EXPECT_NEAR(0.32238, word_dist[1].second, kEpsilon);
    EXPECT_NEAR(0.32238, word_dist[2].second, kEpsilon);

    i = 2;
    doc_words.clear();
    doc_words.push_back("apple");
    doc_words.push_back("orange");
    doc_words.push_back("dog");
    doc_words.push_back("haha");
    explainer_->Explain(doc_words, &topic_dist, &word_dist);
    EXPECT_EQ(6u, word_dist.size());
    EXPECT_EQ("orange", word_dist[0].first);
    EXPECT_EQ("apple", word_dist[1].first);
    EXPECT_EQ("banana", word_dist[2].first);
    EXPECT_EQ("dog", word_dist[3].first);
    EXPECT_EQ("cat", word_dist[4].first);
    EXPECT_EQ("tiger", word_dist[5].first);
    EXPECT_NEAR(0.22254637, word_dist[0].second, kEpsilon);
    EXPECT_NEAR(0.22254637, word_dist[1].second, kEpsilon);
    EXPECT_NEAR(0.22254637, word_dist[2].second, kEpsilon);
    EXPECT_NEAR(0.11023325, word_dist[3].second, kEpsilon);
    EXPECT_NEAR(0.11023325, word_dist[4].second, kEpsilon);
    EXPECT_NEAR(0.11023325, word_dist[5].second, kEpsilon);
  }

  virtual void TearDown() {}

  Model model_;
  Vocabulary vocab_;
  scoped_ptr<Interpreter> interpreter_;
  scoped_ptr<Explainer> explainer_;
};  // class ExplainerTest

TEST_F(ExplainerTest, SparseLDAGibbsSampler) {
  interpreter_.reset(new SparseLDAGibbsSampler(model_, vocab_, 0, 10, 5));
  TestExplain();
}

TEST_F(ExplainerTest, SparseLDAHillClimber) {
  interpreter_.reset(new SparseLDAHillClimber(model_, vocab_, 5, 10));
  TestExplain();
}

TEST_F(ExplainerTest, MultiChainsGibbsSampler) {
  interpreter_.reset(new MultiChainsGibbsSampler(model_, vocab_, 5, 5, 10, 5));
  TestExplainMultiChain();
}

TEST_F(ExplainerTest, MultiTrialsHillClimber) {
  interpreter_.reset(new MultiTrialsHillClimber(model_, vocab_, 5, 5, 10));
  TestExplain();
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

