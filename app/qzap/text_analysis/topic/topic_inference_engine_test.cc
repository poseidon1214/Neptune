// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/topic/topic_inference_engine.h"

#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/text_miner.pb.h"
#include "app/qzap/text_analysis/topic/inference/explainer.h"

DECLARE_int32(peacock_topic_top_k);
DECLARE_int32(peacock_topic_word_top_k);

namespace qzap {
namespace text_analysis {

void CreateDocument(Document* document) {
  Field* field = document->add_field();
  field->set_text("菜汤上浮着一层油会使菜汤凉的快还是慢啊?");
  field->set_weight(1.0);
  field->set_type(TITLE);

  Token* bow_token = document->add_bow_token();
  bow_token->set_text("菜汤");
  bow_token->set_tf(2);
  bow_token = document->add_bow_token();
  bow_token->set_text("上浮");
  bow_token = document->add_bow_token();
  bow_token->set_text("着");
  bow_token = document->add_bow_token();
  bow_token->set_text("一");
  bow_token = document->add_bow_token();
  bow_token->set_text("层");
  bow_token = document->add_bow_token();
  bow_token->set_text("油");
  bow_token = document->add_bow_token();
  bow_token->set_text("会");
  bow_token = document->add_bow_token();
  bow_token->set_text("使");
  bow_token = document->add_bow_token();
  bow_token->set_text("凉");
  bow_token = document->add_bow_token();
  bow_token->set_text("的");
  bow_token = document->add_bow_token();
  bow_token->set_text("快");
  bow_token = document->add_bow_token();
  bow_token->set_text("还是");
  bow_token = document->add_bow_token();
  bow_token->set_text("慢");
  bow_token = document->add_bow_token();
  bow_token->set_text("啊");
  bow_token = document->add_bow_token();
  bow_token->set_text("?");

  document->set_has_extracted_token(true);
}

class TopicInferenceEngineTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    topic_word_explainer_.reset(new TopicInferenceEngine());
    ASSERT_TRUE(topic_word_explainer_->LoadModel("testdata/peacockmodel"));
  }

  scoped_ptr<TopicInferenceEngine> topic_word_explainer_;
};

const static double kEpsilon = 1E-6;

TEST_F(TopicInferenceEngineTest, InferAndExplain) {
  FLAGS_peacock_topic_top_k = 20;
  FLAGS_peacock_topic_word_top_k = 30;

  Document document;
  CreateDocument(&document);

  topic_word_explainer_->InferAndExplain(&document);
  ASSERT_EQ(30, document.topic_word_size());

  EXPECT_EQ("吃", document.topic_word(0).text());
  EXPECT_NEAR(0.90137374, document.topic_word(0).weight(), kEpsilon);
  EXPECT_NEAR(0.061678, document.topic_word(0).ori_weight(), kEpsilon);
  EXPECT_EQ("做法", document.topic_word(1).text());
  EXPECT_NEAR(0.17238015, document.topic_word(1).weight(), kEpsilon);
  EXPECT_NEAR(0.011795, document.topic_word(1).ori_weight(), kEpsilon);
}

TEST_F(TopicInferenceEngineTest, InferAndExplainEmptyText) {
  Document document;
  Field* field = document.add_field();
  field->set_text("");

  EXPECT_TRUE(topic_word_explainer_->InferAndExplain(&document));
  EXPECT_EQ(0, document.topic_word_size());
}

}  // namespace text_analysis
}  // namespace qzap
