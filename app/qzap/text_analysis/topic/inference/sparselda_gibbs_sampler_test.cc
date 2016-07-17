// Copyright (c) 2013 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         YAN Hao (charlieyan@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/inference/sparselda_gibbs_sampler.h"

#include <string>
#include <vector>

#include "app/qzap/common/base/scoped_ptr.h"
#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/topic/base/common.h"
#include "app/qzap/text_analysis/topic/base/model.h"
#include "app/qzap/text_analysis/topic/base/vocabulary.h"

using std::string;
using std::vector;

namespace qzap {
namespace text_analysis {
namespace base {

const static char kModelDir[] = "testdata/model-standard";
const static char kVocabFile[] = "testdata/document_test.vocab";
const static double kEpsilon = 1E-6;

class SparseLDAGibbsSamplerTest : public ::testing::Test {
 public:
  SparseLDAGibbsSamplerTest() : model_(0) {}

  virtual ~SparseLDAGibbsSamplerTest() {}

  virtual void SetUp() {
    CHECK_EQ(0, model_.Load(kModelDir));
    CHECK(vocab_.Load(kVocabFile));
  }

  void TestInterpret() {
    std::vector<std::string> doc_tokens;
    std::vector<std::pair<int32_t, double> > topic_dist;

    size_t i = 0;
    sampler_->Interpret(doc_tokens, &topic_dist);
    EXPECT_EQ(0u, topic_dist.size());

    i = 1;
    doc_tokens.push_back("apple");
    doc_tokens.push_back("orange");
    doc_tokens.push_back("banana");
    sampler_->Interpret(doc_tokens, &topic_dist);
    EXPECT_EQ(1u, topic_dist.size());
    EXPECT_EQ(0, topic_dist[0].first);
    EXPECT_NEAR(0.96875, topic_dist[0].second, kEpsilon);

    i = 2;
    doc_tokens.clear();
    doc_tokens.push_back("apple");
    doc_tokens.push_back("orange");
    doc_tokens.push_back("dog");
    doc_tokens.push_back("haha");
    sampler_->Interpret(doc_tokens, &topic_dist);
    EXPECT_EQ(2u, topic_dist.size());
    EXPECT_EQ(0, topic_dist[0].first);
    EXPECT_NEAR(0.65625, topic_dist[0].second, kEpsilon);
    EXPECT_EQ(1, topic_dist[1].first);
    EXPECT_NEAR(0.34375, topic_dist[1].second, kEpsilon);
  }

  virtual void TearDown() {}

  Model model_;
  Vocabulary vocab_;
  scoped_ptr<SparseLDAGibbsSampler> sampler_;
};  // class SparseLDAGibbsSamplerTest

TEST_F(SparseLDAGibbsSamplerTest, NotCache_Interpret) {
  sampler_.reset(new SparseLDAGibbsSampler(model_, vocab_, 0, 10, 5));
  TestInterpret();
}

TEST_F(SparseLDAGibbsSamplerTest, Cache_Interpret) {
  sampler_.reset(new SparseLDAGibbsSampler(model_, vocab_, 1, 10, 5));
  TestInterpret();
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
