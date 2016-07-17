// Copyright (c) 2012 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// the unittest of class TextMiner

#include "app/qzap/text_analysis/text_miner.h"

#include "app/qzap/common/thread/threadpool.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

#include "app/qzap/text_analysis/dict/word_embedding_dict.h"
#include "app/qzap/text_analysis/dict/keyword_dict.h"
#include "app/qzap/text_analysis/dict/stopword_dict.h"
#include "app/qzap/text_analysis/dict/token_idf_dict.h"
#include "app/qzap/text_analysis/dict_manager.h"
#include "app/qzap/text_analysis/text_miner.pb.h"
#include "app/qzap/text_analysis/text_miner_resource.h"

DECLARE_string(segmenter_data_dir);
DECLARE_string(text_miner_resource_config_file);
DECLARE_bool(extract_keytokens_only);  // default = true
DECLARE_double(classifier_threshold);
DECLARE_double(hierarchical_classifier_threshold);
DECLARE_int32(peacock_topic_top_k);
DECLARE_int32(peacock_topic_word_top_k);
DECLARE_int32(peacock_cache_size_mb);
DECLARE_int32(peacock_num_markov_chains);
DECLARE_int32(peacock_total_iterations);
DECLARE_int32(peacock_burn_in_iterations);

DEFINE_int32(thread_num, 20, "the thread number for testing memory cost");
DEFINE_int32(round, 10, "the loop number of testing");

namespace qzap {
namespace text_analysis {

static const double kEpsilon = 1E-5;

void CreateDocument(Document* document) {
  Field* field = document->add_field();
  field->set_text("鲜花快递");
  field->set_weight(1.0);
  field->set_type(TITLE);

  field = document->add_field();
  field->set_text("鲜花快递，可以选择中国鲜花速递网！");
  field->set_weight(0.5);
  field->set_type(SUMMARY);
}

class TextMinerTest : public ::testing::Test {
 public:
  // functions for threadsafe testing, and multithreads will share
  // one TextMiner object.
  void ThreadsafeSegment(const TextMiner* text_miner) {
    for (int32_t i = 0; i < FLAGS_round; ++i) {
      Document* document = new Document();
      CreateDocument(document);
      text_miner->Segment(document);
      delete document;
    }
  }

  void ThreadsafeExtractTokens(const TextMiner* text_miner) {
    for (int32_t i = 0; i < FLAGS_round; ++i) {
      Document* document = new Document();
      CreateDocument(document);
      text_miner->ExtractTokens(document);
      delete document;
    }
  }

  void ThreadsafeExtractKeywords(const TextMiner* text_miner) {
    for (int32_t i = 0; i < FLAGS_round; ++i) {
      Document* document = new Document();
      CreateDocument(document);
      text_miner->ExtractKeywords(document);
      delete document;
    }
  }

  void ThreadsafeInferTopics(const TextMiner* text_miner) {
    for (int32_t i = 0; i < FLAGS_round; ++i) {
      Document* document = new Document();
      CreateDocument(document);
      text_miner->InferTopics(document);
      delete document;
    }
  }

  void ThreadsafeExplainTopicWords(const TextMiner* text_miner) {
    for (int32_t i = 0; i < FLAGS_round; ++i) {
      Document* document = new Document();
      CreateDocument(document);
      text_miner->ExplainTopicWords(document);
      delete document;
    }
  }

  void ThreadsafeClassify(const TextMiner* text_miner) {
    for (int32_t i = 0; i < FLAGS_round; ++i) {
      Document* document = new Document();
      CreateDocument(document);
      text_miner->Classify(document);
      delete document;
    }
  }

  void ThreadsafeTextMinerFuncs(const TextMiner* text_miner) {
    for (int32_t i = 0; i < FLAGS_round; ++i) {
      Document* document = new Document();
      CreateDocument(document);
      text_miner->Segment(document);
      text_miner->ExtractTokens(document);
      text_miner->ExtractKeywords(document);
      text_miner->InferTopics(document);
      text_miner->ExplainTopicWords(document);
      text_miner->Classify(document);
      delete document;
    }
  }

 protected:
  virtual void SetUp() {
    text_miner_resource_.reset(new TextMinerResource());
    text_miner_resource_->InitFromConfigFile(
        FLAGS_text_miner_resource_config_file);
    text_miner_.reset(new TextMiner(text_miner_resource_.get()));
  }

  scoped_ptr<TextMinerResource> text_miner_resource_;
  scoped_ptr<TextMiner> text_miner_;
};

TEST_F(TextMinerTest, Segment) {
  Document* document = new Document();
  CreateDocument(document);

  ASSERT_TRUE(text_miner_->Segment(document));

  ASSERT_TRUE(document->has_segmented());
  ASSERT_EQ(2, document->field_size());

  const Field& field1 = document->field(0);
  ASSERT_EQ(2, field1.token_size());
  EXPECT_EQ("鲜花", field1.token(0).text());
  EXPECT_EQ("快递", field1.token(1).text());
  EXPECT_EQ("n", field1.token(0).word_type());
  EXPECT_EQ("vn", field1.token(1).word_type());
  EXPECT_EQ(0U, field1.token(0).offset());
  EXPECT_EQ(6U, field1.token(1).offset());
  EXPECT_FALSE(field1.token(0).is_stopword());
  EXPECT_FALSE(field1.token(1).is_stopword());

  const Field& field2 = document->field(1);
  ASSERT_EQ(10, field2.token_size());

  EXPECT_EQ("鲜花", field2.token(0).text());
  EXPECT_EQ("快递", field2.token(1).text());
  EXPECT_EQ(",", field2.token(2).text());
  EXPECT_EQ("可以", field2.token(3).text());
  EXPECT_EQ("选择", field2.token(4).text());
  EXPECT_EQ("中国", field2.token(5).text());
  EXPECT_EQ("鲜花", field2.token(6).text());
  EXPECT_EQ("速递", field2.token(7).text());
  EXPECT_EQ("网", field2.token(8).text());
  EXPECT_EQ("!", field2.token(9).text());

  EXPECT_EQ("n", field2.token(0).word_type());
  EXPECT_EQ("vn", field2.token(1).word_type());
  EXPECT_EQ("w", field2.token(2).word_type());
  EXPECT_EQ("v", field2.token(3).word_type());
  EXPECT_EQ("v", field2.token(4).word_type());
  EXPECT_EQ("ns", field2.token(5).word_type());
  EXPECT_EQ("n", field2.token(6).word_type());
  EXPECT_EQ("vn", field2.token(7).word_type());
  EXPECT_EQ("n", field2.token(8).word_type());
  EXPECT_EQ("w", field2.token(9).word_type());

  EXPECT_EQ(0U, field2.token(0).offset());
  EXPECT_EQ(6U, field2.token(1).offset());
  EXPECT_EQ(12U, field2.token(2).offset());
  EXPECT_EQ(13U, field2.token(3).offset());
  EXPECT_EQ(19U, field2.token(4).offset());
  EXPECT_EQ(25U, field2.token(5).offset());
  EXPECT_EQ(31U, field2.token(6).offset());
  EXPECT_EQ(37U, field2.token(7).offset());
  EXPECT_EQ(43U, field2.token(8).offset());
  EXPECT_EQ(46U, field2.token(9).offset());

  EXPECT_FALSE(field2.token(0).is_stopword());
  EXPECT_FALSE(field2.token(1).is_stopword());
  EXPECT_TRUE(field2.token(2).is_stopword());
  EXPECT_TRUE(field2.token(3).is_stopword());
  EXPECT_FALSE(field2.token(4).is_stopword());
  EXPECT_FALSE(field2.token(5).is_stopword());
  EXPECT_FALSE(field2.token(6).is_stopword());
  EXPECT_FALSE(field2.token(7).is_stopword());
  EXPECT_FALSE(field2.token(8).is_stopword());
  EXPECT_TRUE(field2.token(9).is_stopword());

  delete document;
}

TEST_F(TextMinerTest, ExtractTokens) {
  Document* document = new Document();
  CreateDocument(document);

  ASSERT_TRUE(text_miner_->ExtractTokens(document));

  ASSERT_EQ(6, document->bow_token_size());
  EXPECT_EQ("鲜花", document->bow_token(0).text());
  EXPECT_EQ("快递", document->bow_token(1).text());
  EXPECT_EQ("选择", document->bow_token(2).text());
  EXPECT_EQ("中国", document->bow_token(3).text());
  EXPECT_EQ("速递", document->bow_token(4).text());
  EXPECT_EQ("网", document->bow_token(5).text());

  EXPECT_EQ(3U, document->bow_token(0).tf());
  EXPECT_EQ(2U, document->bow_token(1).tf());
  EXPECT_EQ(1U, document->bow_token(2).tf());
  EXPECT_EQ(1U, document->bow_token(3).tf());
  EXPECT_EQ(1U, document->bow_token(4).tf());
  EXPECT_EQ(1U, document->bow_token(5).tf());

  EXPECT_NEAR(0.81679612, document->bow_token(0).weight(), kEpsilon);
  EXPECT_NEAR(0.46487647, document->bow_token(1).weight(), kEpsilon);
  EXPECT_NEAR(0.113162, document->bow_token(2).weight(), kEpsilon);
  EXPECT_NEAR(0.19462834, document->bow_token(3).weight(), kEpsilon);
  EXPECT_NEAR(0.20607172, document->bow_token(4).weight(), kEpsilon);
  EXPECT_NEAR(0.15356599, document->bow_token(5).weight(), kEpsilon);

  delete document;
}

TEST_F(TextMinerTest, ExtractKeywords) {
  Document* document = new Document();
  CreateDocument(document);

  ASSERT_TRUE(text_miner_->ExtractKeywords(document));

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

  EXPECT_NEAR(0.77370167, document->bow_keyword(0).weight(), kEpsilon);
  EXPECT_NEAR(0.51580113, document->bow_keyword(1).weight(), kEpsilon);
  EXPECT_NEAR(0.32237571, document->bow_keyword(2).weight(), kEpsilon);
  EXPECT_NEAR(0.08596686, document->bow_keyword(3).weight(), kEpsilon);
  EXPECT_NEAR(0.12895028, document->bow_keyword(4).weight(), kEpsilon);
  EXPECT_NEAR(0.08596686, document->bow_keyword(5).weight(), kEpsilon);

  delete document;
}

TEST_F(TextMinerTest, InferTopics) {
  Document document;
  Field* field = document.add_field();
  field->set_text("菜汤上浮着一层油会使菜汤凉的快还是慢啊?");
  text_miner_->InferTopics(&document);
  ASSERT_EQ(3, document.topic_size());

  EXPECT_EQ(62U, document.topic(0).id());
  EXPECT_NEAR(0.967696, document.topic(0).weight(), kEpsilon);
  EXPECT_EQ(30U, document.topic(1).id());
  EXPECT_NEAR(0.244464, document.topic(1).weight(), kEpsilon);
  EXPECT_EQ(88U, document.topic(2).id());
  EXPECT_NEAR(0.061661, document.topic(2).weight(), kEpsilon);
}

TEST_F(TextMinerTest, InferTopicsEmptyText) {
  Document document;
  Field* field = document.add_field();
  field->set_text("");
  EXPECT_TRUE(text_miner_->InferTopics(&document));
  EXPECT_EQ(0, document.topic_size());
}

TEST_F(TextMinerTest, ExplainTopicWords) {
  Document document;
  Field* field = document.add_field();
  field->set_text("菜汤上浮着一层油会使菜汤凉的快还是慢啊?");
  text_miner_->ExplainTopicWords(&document);
  ASSERT_EQ(30, document.topic_word_size());

  EXPECT_EQ("吃", document.topic_word(0).text());
  EXPECT_NEAR(0.904496, document.topic_word(0).weight(), kEpsilon);
  EXPECT_NEAR(0.067382, document.topic_word(0).ori_weight(), kEpsilon);
  EXPECT_EQ("做法", document.topic_word(1).text());
  EXPECT_NEAR(0.168063, document.topic_word(1).weight(), kEpsilon);
  EXPECT_NEAR(0.012520, document.topic_word(1).ori_weight(), kEpsilon);
}

TEST_F(TextMinerTest, Classify) {
  Document document;
  Field* field = document.add_field();
  field->set_text("中国牛杂火锅目前有好多家分店，经营理验是？");
  text_miner_->Classify(&document);
  ASSERT_EQ(3, document.category_size());

  EXPECT_EQ(101U, document.category(0).id());
  EXPECT_NEAR(0.999999, document.category(0).weight(), kEpsilon);
  EXPECT_EQ(102U, document.category(1).id());
  EXPECT_NEAR(0.000216, document.category(1).weight(), kEpsilon);
  EXPECT_EQ(201U, document.category(2).id());
  EXPECT_NEAR(0.000466, document.category(2).weight(), kEpsilon);

  document.Clear();
  field = document.add_field();
  field->set_text("谁有2010年夏季女装的流行趋势？");
  text_miner_->Classify(&document);
  ASSERT_EQ(3, document.category_size());
  EXPECT_EQ(201U, document.category(0).id());
  EXPECT_NEAR(1.0, document.category(0).weight(), kEpsilon);
  EXPECT_EQ(101U, document.category(1).id());
  EXPECT_NEAR(1.51758e-05, document.category(1).weight(), kEpsilon);
  EXPECT_EQ(102U, document.category(2).id());
  EXPECT_NEAR( 1.515939e-05, document.category(2).weight(), kEpsilon);
}

TEST_F(TextMinerTest, InferEmbedding) {
  Document document;
  Token* bow_token = document.add_bow_token();
  bow_token->set_text("游戏");
  bow_token->set_weight(10);
  bow_token = document.add_bow_token();
  bow_token->set_text("爱好");
  bow_token->set_weight(5);
  bow_token = document.add_bow_token();
  bow_token->set_text("群");
  bow_token->set_weight(0.5);

  document.set_has_extracted_token(true);

  text_miner_->InferEmbedding(&document);
  ASSERT_EQ(50, document.embedding_size());

  EXPECT_NEAR(0.203496337, document.embedding(0).weight(), kEpsilon);
  EXPECT_NEAR(-0.37111834, document.embedding(1).weight(), kEpsilon);
  EXPECT_NEAR(-0.05158433, document.embedding(2).weight(), kEpsilon);
  EXPECT_NEAR(-0.08381233, document.embedding(3).weight(), kEpsilon);
}

TEST_F(TextMinerTest, ClassifyEmptyText) {
  Document document;
  Field* field = document.add_field();
  field->set_text("");
  EXPECT_TRUE(text_miner_->Classify(&document));
  EXPECT_EQ(0, document.category_size());
}

TEST_F(TextMinerTest, ThreadsafeSegment) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("ThreadsafeSegment",
                                              FLAGS_thread_num));
  p->Start();
  for (int32_t i = 0; i < FLAGS_round * FLAGS_thread_num; ++i) {
    p->PushTask(NewCallback(this,
                            &TextMinerTest::ThreadsafeSegment,
                            text_miner_.get()));
  }
  p->Stop();
}

TEST_F(TextMinerTest, ThreadsafeExtractTokens) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("ThreadsafeExtractTokens",
                                              FLAGS_thread_num));
  p->Start();
  for (int32_t i = 0; i < FLAGS_round * FLAGS_thread_num; ++i) {
    p->PushTask(NewCallback(this,
                            &TextMinerTest::ThreadsafeExtractTokens,
                            text_miner_.get()));
  }
  p->Stop();
}

TEST_F(TextMinerTest, ThreadsafeExtractKeywords) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("ThreadsafeExtractKeywords",
                                              FLAGS_thread_num));
  p->Start();
  for (int32_t i = 0; i < FLAGS_round * FLAGS_thread_num; ++i) {
    p->PushTask(NewCallback(this,
                            &TextMinerTest::ThreadsafeExtractKeywords,
                            text_miner_.get()));
  }
  p->Stop();
}

TEST_F(TextMinerTest, ThreadsafeInferTopics) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("ThreadsafeInferTopics",
                                              FLAGS_thread_num));
  p->Start();
  for (int32_t i = 0; i < FLAGS_round * FLAGS_thread_num; ++i) {
    p->PushTask(NewCallback(this,
                            &TextMinerTest::ThreadsafeInferTopics,
                            text_miner_.get()));
  }
  p->Stop();
}

TEST_F(TextMinerTest, ThreadsafeExplainTopicWords) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("ThreadsafeExplainTopicWords",
                                              FLAGS_thread_num));
  p->Start();
  for (int32_t i = 0; i < FLAGS_round * FLAGS_thread_num; ++i) {
    p->PushTask(NewCallback(this,
                            &TextMinerTest::ThreadsafeExplainTopicWords,
                            text_miner_.get()));
  }
  p->Stop();
}

TEST_F(TextMinerTest, ThreadsafeClassify) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("ThreadsafeClassify",
                                              FLAGS_thread_num));
  p->Start();
  for (int32_t i = 0; i < FLAGS_round * FLAGS_thread_num; ++i) {
    p->PushTask(NewCallback(this,
                            &TextMinerTest::ThreadsafeClassify,
                            text_miner_.get()));
  }
  p->Stop();
}

TEST_F(TextMinerTest, ThreadsafeTextMinerFuncs) {
  shared_ptr<ThreadPool> p(ThreadPool::Create("ThreadsafeTextMinerFuncs",
                                              FLAGS_thread_num));
  p->Start();
  for (int32_t i = 0; i < FLAGS_round * FLAGS_thread_num; ++i) {
    p->PushTask(NewCallback(this,
                            &TextMinerTest::ThreadsafeTextMinerFuncs,
                            text_miner_.get()));
  }
  p->Stop();
}

}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);
  ::testing::InitGoogleTest(&argc, argv);

  FLAGS_segmenter_data_dir = "testdata/tc_data";
  FLAGS_text_miner_resource_config_file = "testdata/text_miner_resource.config";
  FLAGS_classifier_threshold = 0.0;
  FLAGS_hierarchical_classifier_threshold = 0.0;

  FLAGS_peacock_cache_size_mb = 5 * 1024;
  FLAGS_peacock_num_markov_chains = 5;
  FLAGS_peacock_total_iterations = 15;
  FLAGS_peacock_burn_in_iterations = 10;
  FLAGS_peacock_topic_top_k = 20;
  FLAGS_peacock_topic_word_top_k = 30;

  return RUN_ALL_TESTS();
}
