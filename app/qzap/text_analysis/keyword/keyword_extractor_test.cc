// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/keyword/keyword_extractor.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

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
  token->set_text("，");
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
  token->set_text("！");
  token->set_word_type("w");
  token->set_offset(48);

  field = document->add_field();
  field->set_text("贺州牙科医院");
  field->set_weight(0.5);
  field->set_type(SUMMARY);

  token = field->add_token();
  token->set_text("贺州");
  token->set_word_type("ns");
  token->set_offset(0);
  token->set_text("牙科");
  token->set_word_type("n");
  token->set_offset(6);
  token->set_text("医院");
  token->set_word_type("n");
  token->set_offset(12);

  document->set_has_segmented(true);
}

TEST(KeywordExtractor, Extract) {
  DictManager dict_manager;
  ASSERT_TRUE(dict_manager.Init("testdata/kedict"));
  KeywordExtractor keyword_extractor(dict_manager);
  Document* document = new Document();
  ASSERT_FALSE(keyword_extractor.Extract(document));

  CreateDocument(document);
  ASSERT_TRUE(keyword_extractor.Extract(document));

  ASSERT_EQ(6, document->bow_keyword_size());
  EXPECT_EQ("鲜花", document->bow_keyword(0).text());
  EXPECT_EQ("鲜花快递", document->bow_keyword(1).text());
  EXPECT_EQ("快递", document->bow_keyword(2).text());
  EXPECT_EQ("中国鲜花速递网", document->bow_keyword(3).text());
  EXPECT_EQ("鲜花速递", document->bow_keyword(4).text());
  EXPECT_EQ("速递", document->bow_keyword(5).text());

  EXPECT_EQ(1, document->bow_keyword(0).token_size());
  EXPECT_EQ("鲜花", document->bow_keyword(0).token(0));
  EXPECT_EQ(2, document->bow_keyword(1).token_size());
  EXPECT_EQ("鲜花", document->bow_keyword(1).token(0));
  EXPECT_EQ("快递", document->bow_keyword(1).token(1));
  EXPECT_EQ(1, document->bow_keyword(2).token_size());
  EXPECT_EQ("快递", document->bow_keyword(2).token(0));
  EXPECT_EQ(4, document->bow_keyword(3).token_size());
  EXPECT_EQ("中国", document->bow_keyword(3).token(0));
  EXPECT_EQ("鲜花", document->bow_keyword(3).token(1));
  EXPECT_EQ("速递", document->bow_keyword(3).token(2));
  EXPECT_EQ("网", document->bow_keyword(3).token(3));
  EXPECT_EQ(2, document->bow_keyword(4).token_size());
  EXPECT_EQ("鲜花", document->bow_keyword(4).token(0));
  EXPECT_EQ("速递", document->bow_keyword(4).token(1));
  EXPECT_EQ(1, document->bow_keyword(5).token_size());
  EXPECT_EQ("速递", document->bow_keyword(5).token(0));

  static const double kEpsilon = 1E-6;
  EXPECT_NEAR(0.77370167, document->bow_keyword(0).weight(), kEpsilon);
  EXPECT_NEAR(0.51580113, document->bow_keyword(1).weight(), kEpsilon);
  EXPECT_NEAR(0.32237571, document->bow_keyword(2).weight(), kEpsilon);
  EXPECT_NEAR(0.08596686, document->bow_keyword(3).weight(), kEpsilon);
  EXPECT_NEAR(0.12895028, document->bow_keyword(4).weight(), kEpsilon);
  EXPECT_NEAR(0.08596686, document->bow_keyword(5).weight(), kEpsilon);

  delete document;
}

}  // namespace content_analyzer
}  // namespace cocktail

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
//  FLAGS_segmenter_data_dir = "testdata/tc_data";

  return RUN_ALL_TESTS();
}
