// Copyright (c) 2015 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/embedding/embedding_inference_engine.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/dict_manager.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

namespace qzap {
namespace text_analysis {

static const double kEpsilon = 1E-6;

void CreateDocument(Document* document) {
  Token* bow_token = document->add_bow_token();
  bow_token->set_text("游戏");
  bow_token->set_weight(10);
  bow_token = document->add_bow_token();
  bow_token->set_text("爱好");
  bow_token->set_weight(5);
  bow_token = document->add_bow_token();
  bow_token->set_text("群");
  bow_token->set_weight(0.5);

  document->set_has_extracted_token(true);
}

TEST(EmbeddingInferenceEngine, Infer) {
  DictManager dict_manager;
  ASSERT_TRUE(dict_manager.Init("testdata/kedict"));

  EmbeddingInferenceEngine embedding_inference_engine(dict_manager);

  Document* document = new Document();
  ASSERT_FALSE(embedding_inference_engine.Infer(document));

  CreateDocument(document);
  ASSERT_TRUE(embedding_inference_engine.Infer(document));

  ASSERT_EQ(50, document->embedding_size());

  EXPECT_NEAR(0.203496337, document->embedding(0).weight(), kEpsilon);
  EXPECT_NEAR(-0.37111834, document->embedding(1).weight(), kEpsilon);
  EXPECT_NEAR(-0.05158433, document->embedding(2).weight(), kEpsilon);
  EXPECT_NEAR(-0.08381233, document->embedding(3).weight(), kEpsilon);

  delete document;
}

}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  ::google::ParseCommandLineFlags(&argc, &argv, true);

  return RUN_ALL_TESTS();
}

