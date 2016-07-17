// Copyright (c) 2012 Tencent Inc.
// Author: Pan Yang (baileyyang@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)
// the unittest of FeatureExtractor class

#include "app/qzap/text_analysis/classifier/feature/feature_extractor.h"

#include "thirdparty/gtest/gtest.h"
#include "app/qzap/text_analysis/classifier/instance.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

DEFINE_string(feature_vocabulary_file, "dict.feature_vocabulary",
              "the binary feature vocabulary file");

namespace qzap {
namespace text_analysis {

TEST(FeatureExtractorTest, ExtractTokenFeature) {
  Document doc;
  Instance instance;
  FeatureExtractor feature_extractor;
  feature_extractor.LoadFeatureVocab(FLAGS_feature_vocabulary_file);

  // test for empty document
  feature_extractor.ExtractTokenFeature(doc, &instance);
  EXPECT_EQ(0u, instance.NumFeatures());

  // test for token feature not in feature vocabulary
  Token* token = doc.add_bow_token();
  token->set_text("google");
  token->set_weight(1.0);
  token = doc.add_bow_token();
  token->set_text("microsoft");
  token->set_weight(1.0);
  token = doc.add_bow_token();
  token->set_text("baidu");
  token->set_weight(1.0);
  token = doc.add_bow_token();
  token->set_text("tencent");
  token->set_weight(1.0);
  feature_extractor.ExtractTokenFeature(doc, &instance);
  EXPECT_EQ(0u, instance.NumFeatures());

  // test for token feature in feature vocabulary
  token = doc.add_bow_token();
  token->set_text("apple");
  token->set_weight(1.0);
  token = doc.add_bow_token();
  token->set_text("ipod");
  token->set_weight(1.0);
  token = doc.add_bow_token();
  token->set_text("iphone");
  token->set_weight(1.0);
  token = doc.add_bow_token();
  token->set_text("ipad");
  token->set_weight(1.0);
  feature_extractor.ExtractTokenFeature(doc, &instance);
  EXPECT_EQ(4u, instance.NumFeatures());
}

TEST(FeatureExtractorTest, ExtractKeywordFeature) {
  Document doc;
  Instance instance;
  FeatureExtractor feature_extractor;
  feature_extractor.LoadFeatureVocab(FLAGS_feature_vocabulary_file);

  // test for empty document
  feature_extractor.ExtractKeywordFeature(doc, &instance);
  EXPECT_EQ(0u, instance.NumFeatures());

  // test for keyword feature not in feature vocabulary
  Keyword* keyword = doc.add_bow_keyword();
  keyword->set_text("google inc");
  keyword->set_weight(1.0);
  keyword = doc.add_bow_keyword();
  keyword->set_text("microsoft inc");
  keyword->set_weight(1.0);
  keyword = doc.add_bow_keyword();
  keyword->set_text("baidu inc");
  keyword->set_weight(1.0);
  keyword = doc.add_bow_keyword();
  keyword->set_text("tencent inc");
  keyword->set_weight(1.0);
  feature_extractor.ExtractKeywordFeature(doc, &instance);
  EXPECT_EQ(0u, instance.NumFeatures());

  // test for keyword feature in feature vocabulary
  keyword = doc.add_bow_keyword();
  keyword->set_text("iphone 4s");
  keyword->set_weight(1.0);
  keyword = doc.add_bow_keyword();
  keyword->set_text("ipod touch");
  keyword->set_weight(1.0);
  keyword = doc.add_bow_keyword();
  keyword->set_text("ipad 2");
  keyword->set_weight(1.0);
  feature_extractor.ExtractKeywordFeature(doc, &instance);
  EXPECT_EQ(3u, instance.NumFeatures());
}

TEST(FeatureExtractorTest, ExtractTopicFeature) {
  Document doc;
  Instance instance;
  FeatureExtractor feature_extractor;
  feature_extractor.LoadFeatureVocab(FLAGS_feature_vocabulary_file);

  // test for empty document
  feature_extractor.ExtractTopicFeature(doc, &instance);
  EXPECT_EQ(0u, instance.NumFeatures());

  // test for lda feature not in feature vocabulary
  Topic* topic = doc.add_topic();
  topic->set_id(0);
  topic->set_weight(1.0);
  topic = doc.add_topic();
  topic->set_id(9);
  topic->set_weight(1.0);
  topic = doc.add_topic();
  topic->set_id(99);
  topic->set_weight(1.0);
  feature_extractor.ExtractTopicFeature(doc, &instance);
  EXPECT_EQ(0u, instance.NumFeatures());

  // test for lda feature in feature vocabulary
  topic = doc.add_topic();
  topic->set_id(1);
  topic->set_weight(1.0);
  topic = doc.add_topic();
  topic->set_id(10);
  topic->set_weight(1.0);
  topic = doc.add_topic();
  topic->set_id(100);
  topic->set_weight(1.0);
  feature_extractor.ExtractTopicFeature(doc, &instance);
  EXPECT_EQ(3u, instance.NumFeatures());
}

TEST(FeatureExtractorTest, ExtractEmbeddingFeature) {
  Document doc;
  Instance instance;
  FeatureExtractor feature_extractor;
  feature_extractor.LoadFeatureVocab(FLAGS_feature_vocabulary_file);

  // test for empty document
  feature_extractor.ExtractEmbeddingFeature(doc, &instance);
  EXPECT_EQ(0u, instance.NumFeatures());

  Embedding* embedding = doc.add_embedding();
  embedding->set_weight(0.8);
  embedding = doc.add_embedding();
  embedding->set_weight(0.5);
  embedding = doc.add_embedding();
  embedding->set_weight(0.3);

  feature_extractor.ExtractEmbeddingFeature(doc, &instance);
  EXPECT_EQ(3u, instance.NumFeatures());
}

}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  FLAGS_feature_vocabulary_file = "testdata/dict.feature_vocabulary";

  return RUN_ALL_TESTS();
}
