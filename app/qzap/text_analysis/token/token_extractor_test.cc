// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/token/token_extractor.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/dict_manager.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

DECLARE_bool(extract_keytokens_only);

namespace qzap {
namespace text_analysis {

void CreateDocument(Document* document) {
  Field* field = document->add_field();
  field->set_text("鲜花快递");
  field->set_weight(1.0);
  field->set_type(TITLE);
  TokenOccurence* token = field->add_token();
  token->set_text("鲜花");
  token->set_word_type("n");
  token->set_offset(0);
  token = field->add_token();
  token->set_text("快递");
  token->set_word_type("vn");
  token->set_offset(6);

  field = document->add_field();
  field->set_text("鲜花快递，可以选择中国鲜花速递网！");
  field->set_weight(0.5);
  field->set_type(SUMMARY);
  token = field->add_token();
  token->set_text("鲜花");
  token->set_word_type("n");
  token->set_offset(0);
  token = field->add_token();
  token->set_text("快递");
  token->set_word_type("vn");
  token->set_offset(6);
  token = field->add_token();
  token->set_text(",");
  token->set_word_type("w");
  token->set_offset(12);
  token = field->add_token();
  token->set_text("可以");
  token->set_word_type("v");
  token->set_offset(15);
  token->set_is_stopword(true);
  token = field->add_token();
  token->set_text("选择");
  token->set_word_type("v");
  token->set_offset(21);
  token = field->add_token();
  token->set_text("中国");
  token->set_word_type("ns");
  token->set_offset(27);
  token = field->add_token();
  token->set_text("鲜花");
  token->set_word_type("n");
  token->set_offset(33);
  token = field->add_token();
  token->set_text("速递");
  token->set_word_type("vn");
  token->set_offset(39);
  token = field->add_token();
  token->set_text("网");
  token->set_word_type("n");
  token->set_offset(45);
  token = field->add_token();
  token->set_text("!");
  token->set_word_type("w");
  token->set_offset(48);

  document->set_has_segmented(true);
}

const static double kEpsilon = 1E-6;

TEST(TokenExtractor, ExtractAllTokens) {
  FLAGS_extract_keytokens_only = false;

  DictManager dict_manager;
  ASSERT_TRUE(dict_manager.Init("testdata/kedict"));

  TokenExtractor token_extractor(dict_manager);
  Document* document = new Document();
  ASSERT_FALSE(token_extractor.Extract(document));

  CreateDocument(document);
  ASSERT_TRUE(token_extractor.Extract(document));

  ASSERT_EQ(9, document->bow_token_size());
  EXPECT_EQ("鲜花", document->bow_token(0).text());
  EXPECT_EQ("快递", document->bow_token(1).text());
  EXPECT_EQ(",", document->bow_token(2).text());
  EXPECT_EQ("可以", document->bow_token(3).text());
  EXPECT_EQ("选择", document->bow_token(4).text());
  EXPECT_EQ("中国", document->bow_token(5).text());
  EXPECT_EQ("速递", document->bow_token(6).text());
  EXPECT_EQ("网", document->bow_token(7).text());
  EXPECT_EQ("!", document->bow_token(8).text());

  EXPECT_NEAR(0.81679612, document->bow_token(0).weight(), kEpsilon);
  EXPECT_NEAR(0.46487647, document->bow_token(1).weight(), kEpsilon);
  EXPECT_NEAR(0, document->bow_token(2).weight(), kEpsilon);
  EXPECT_NEAR(0, document->bow_token(3).weight(), kEpsilon);
  EXPECT_NEAR(0.113162, document->bow_token(4).weight(), kEpsilon);
  EXPECT_NEAR(0.194628, document->bow_token(5).weight(), kEpsilon);
  EXPECT_NEAR(0.206072, document->bow_token(6).weight(), kEpsilon);
  EXPECT_NEAR(0.153566, document->bow_token(7).weight(), kEpsilon);
  EXPECT_NEAR(0, document->bow_token(8).weight(), kEpsilon);

  delete document;
}

TEST(TokenExtractor, ExtractKeyTokens) {
  DictManager dict_manager;
  ASSERT_TRUE(dict_manager.Init("testdata/kedict"));
  TokenExtractor token_extractor(dict_manager);
  Document* document = new Document();
  CreateDocument(document);
  FLAGS_extract_keytokens_only = true;

  ASSERT_TRUE(token_extractor.Extract(document));

  ASSERT_EQ(6, document->bow_token_size());
  EXPECT_EQ("鲜花", document->bow_token(0).text());
  EXPECT_EQ("快递", document->bow_token(1).text());
  EXPECT_EQ("选择", document->bow_token(2).text());
  EXPECT_EQ("中国", document->bow_token(3).text());
  EXPECT_EQ("速递", document->bow_token(4).text());
  EXPECT_EQ("网", document->bow_token(5).text());

  EXPECT_NEAR(0.816796, document->bow_token(0).weight(), kEpsilon);
  EXPECT_NEAR(0.464876, document->bow_token(1).weight(), kEpsilon);
  EXPECT_NEAR(0.113162, document->bow_token(2).weight(), kEpsilon);
  EXPECT_NEAR(0.194628, document->bow_token(3).weight(), kEpsilon);
  EXPECT_NEAR(0.206072, document->bow_token(4).weight(), kEpsilon);
  EXPECT_NEAR(0.153566, document->bow_token(5).weight(), kEpsilon);

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

