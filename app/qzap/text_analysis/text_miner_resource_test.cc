// Copyright (c) 2012 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
// Author: YiXiang Mu (javiermu@tencent.com)
//
// the unittest of class TextMinerResource

#include "app/qzap/text_analysis/text_miner_resource.h"

#include <string>
#include <tr1/memory>
#include <vector>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

#include "app/qzap/text_analysis/segmenter/segmenter.h"
#include "app/qzap/text_analysis/dict/keyword_dict.h"
#include "app/qzap/text_analysis/dict/stopword_dict.h"
#include "app/qzap/text_analysis/dict/token_idf_dict.h"
#include "app/qzap/text_analysis/dict_manager.h"
#include "app/qzap/text_analysis/embedding/embedding_inference_engine.h"
#include "app/qzap/text_analysis/keyword/keyword_extractor.h"
#include "app/qzap/text_analysis/text_miner.pb.h"
#include "app/qzap/text_analysis/token/token_extractor.h"
#include "app/qzap/text_analysis/topic/topic_inference_engine.h"

DECLARE_string(segmenter_data_dir);
DECLARE_string(text_miner_resource_config_file);

namespace qzap {
namespace text_analysis {

static const double kEpsilon = 1E-6;

TEST(TextMinerResource, Init) {
  scoped_ptr<TextMinerResource> text_miner_resource;
  text_miner_resource.reset(new TextMinerResource());
  EXPECT_TRUE(text_miner_resource->InitFromConfigFile(
          FLAGS_text_miner_resource_config_file));

  text_miner_resource.reset(new TextMinerResource());
  ResourceConfig resource_config;
  resource_config.set_dict_dir("testdata/kedict");
  resource_config.set_peacock_model_dir("testdata/peacockmodel");
  resource_config.set_classifier_model_dir(
      "testdata/classifier_model");
  EXPECT_TRUE(text_miner_resource->InitFromPbMessage(resource_config));
}

class TextMinerResourceTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    text_miner_resource_.reset(new TextMinerResource());
    text_miner_resource_->InitFromConfigFile(
        "testdata/text_miner_resource.config");
  }

  scoped_ptr<TextMinerResource> text_miner_resource_;
};

TEST_F(TextMinerResourceTest, GetDictManager) {
  const DictManager* dict_manager_ptr = text_miner_resource_->GetDictManager();
  ASSERT_TRUE(dict_manager_ptr != NULL);

  const KeywordDict* keyword_dict_ptr = dict_manager_ptr->GetKeywordDict();
  ASSERT_TRUE(keyword_dict_ptr != NULL);
  const KeywordInfo* value = keyword_dict_ptr->Search("Nokia手机");
  EXPECT_TRUE(value != NULL);

  const StopwordDict* stopword_dict_ptr = dict_manager_ptr->GetStopwordDict();
  ASSERT_TRUE(stopword_dict_ptr != NULL);
  EXPECT_TRUE(stopword_dict_ptr->IsStopword("可以"));
  EXPECT_FALSE(stopword_dict_ptr->IsStopword("凡客"));

  const TokenIdfDict* token_idf_dict_ptr = dict_manager_ptr->GetTokenIdfDict();
  ASSERT_TRUE(token_idf_dict_ptr != NULL);
  const IdfInfo* idf_info = token_idf_dict_ptr->Search("酒店");
  ASSERT_TRUE(idf_info != NULL);
  EXPECT_TRUE(token_idf_dict_ptr->Search("那么") == NULL);

  const WordEmbeddingDict* word_embedding_dict_ptr
      = dict_manager_ptr->GetWordEmbeddingDict();
  ASSERT_TRUE(word_embedding_dict_ptr != NULL);
  const EmbeddingInfo* embedding_info = word_embedding_dict_ptr->Search("朋友");
  EXPECT_TRUE(embedding_info != NULL);
  EXPECT_EQ(50, embedding_info->embedding_size());
  EXPECT_NEAR(embedding_info->embedding(0), 0.349071, kEpsilon);
  EXPECT_TRUE(word_embedding_dict_ptr->Search("腾讯") == NULL);
}

TEST_F(TextMinerResourceTest, GetSegmenter) {
  const Segmenter* segmenter = text_miner_resource_->GetSegmenter();
  ASSERT_TRUE(segmenter != NULL);

  std::vector<std::string> tokens;
  EXPECT_TRUE(segmenter->Segment("鲜花快递", &tokens));
  ASSERT_EQ(2U, tokens.size());
  EXPECT_EQ("鲜花", tokens[0]);
  EXPECT_EQ("快递", tokens[1]);

  tokens.clear();
  std::vector<std::string> word_types;
  EXPECT_TRUE(segmenter->SegmentWithWordType("鲜花快递", &tokens, &word_types));
  ASSERT_EQ(2U, tokens.size());
  EXPECT_EQ("鲜花", tokens[0]);
  EXPECT_EQ("快递", tokens[1]);
  ASSERT_EQ(2U, word_types.size());
  EXPECT_EQ("n", word_types[0]);
  EXPECT_EQ("vn", word_types[1]);
}

TEST_F(TextMinerResourceTest, GetTokenExtractor) {
  const TokenExtractor* token_extractor
      = text_miner_resource_->GetTokenExtractor();
  EXPECT_TRUE(token_extractor != NULL);
}

TEST_F(TextMinerResourceTest, GetKeywordExtractor) {
  const KeywordExtractor* keyword_extractor
      = text_miner_resource_->GetKeywordExtractor();
  EXPECT_TRUE(keyword_extractor != NULL);
}

TEST_F(TextMinerResourceTest, GetTopicInferenceEngine) {
  const TopicInferenceEngine* topic_inference_engine
      = text_miner_resource_->GetTopicInferenceEngine();
  EXPECT_TRUE(topic_inference_engine != NULL);
}

TEST_F(TextMinerResourceTest, GetClassifier) {
  const Classifier* classifier = text_miner_resource_->GetClassifier();
  EXPECT_TRUE(classifier != NULL);
}

TEST_F(TextMinerResourceTest, GetEmbeddingInferenceEngine) {
  const EmbeddingInferenceEngine* embedding_inference_engine
      = text_miner_resource_->GetEmbeddingInferenceEngine();
  EXPECT_TRUE(embedding_inference_engine != NULL);
}

}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);
  ::testing::InitGoogleTest(&argc, argv);

  FLAGS_segmenter_data_dir = "testdata/tc_data";
  FLAGS_text_miner_resource_config_file = "testdata/text_miner_resource.config";

  return RUN_ALL_TESTS();
}
