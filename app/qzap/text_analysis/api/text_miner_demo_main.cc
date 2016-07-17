// Copyright (c) 2015 Tencent Inc.
// Author: Ping Wang (samulwang@tencent.com)
//
// 演示如何使用libspa_text_miner.so，调用了几个主要API

#include <iostream> // NOLINT

#include "text_miner_resource.h"
#include "text_miner.h"

using namespace qzap::text_analysis;

static const char* FLAGS_feature_separator = "  ";
static const char* FLAGS_weight_separator  = ",";

void MockDoc(Document& document) {
  Field* field = document.add_field();
  field->set_text("鲜花快递");
  field->set_weight(1.0);
  field->set_type(TITLE);

  field = document.add_field();
  field->set_text("鲜花快递，可以选择中国鲜花速递网！");
  field->set_weight(0.5);
  field->set_type(SUMMARY);
}

bool ExtractTokens(TextMiner* text_miner,
                   Document* document,
                   std::ostream& os) {
  if (!text_miner->ExtractTokens(document)) {
    os << std::endl;
    return false;
  }
  for (int i = 0; i < document->bow_token_size(); ++i) {
    os << document->bow_token(i).text() << FLAGS_weight_separator
        << document->bow_token(i).weight()
        << (i + 1 == document->bow_token_size() ? "" :
            FLAGS_feature_separator);
  }
  os << std::endl;
  return true;
}

bool ExtractKeywords(TextMiner* text_miner,
                     Document* document,
                     std::ostream& os) {
  if (!text_miner->ExtractKeywords(document)) {
    os << std::endl;
    return false;
  }
  for (int i = 0; i < document->bow_keyword_size(); ++i) {
    os << document->bow_keyword(i).text() << FLAGS_weight_separator
        << document->bow_keyword(i).weight()
        << (i + 1 == document->bow_keyword_size() ? "" :
            FLAGS_feature_separator);
  }
  os << std::endl;
  return true;
}

bool ExtractTopics(TextMiner* text_miner, Document* document, std::ostream& os) {
  if (!text_miner->InferTopics(document)) {
    os << std::endl;
    return false;
  }
  for (int i = 0; i < document->topic_size(); ++i) {
    os << document->topic(i).id() << FLAGS_weight_separator
        << document->topic(i).weight()
        << (i + 1 == document->topic_size() ? "" : FLAGS_feature_separator);
  }
  os << std::endl;
  return true;
}

bool ExtractTopicWords(TextMiner* text_miner,
                       Document* document,
                       std::ostream& os) {
  if (!text_miner->ExplainTopicWords(document)) {
    os << std::endl;
    return false;
  }
  for (int i = 0; i < document->topic_word_size(); ++i) {
    os << document->topic_word(i).text() << FLAGS_weight_separator
        << document->topic_word(i).weight()
        << (i + 1 == document->topic_word_size() ? "" :
            FLAGS_feature_separator);
  }
  os << std::endl;
  return true;
}

bool ExtractCategories(TextMiner* text_miner,
                       Document* document,
                       std::ostream& os) {
  if (!text_miner->Classify(document)) {
    os << std::endl;
    return false;
  }
  for (int i = 0; i < document->category_size(); ++i) {
    os << document->category(i).id() << FLAGS_weight_separator
        << document->category(i).weight()
        << (i + 1 == document->category_size() ? "" :
            FLAGS_feature_separator);
  }
  os << std::endl;
  return true;
}

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging(argv[0]);

  TextMinerResource resource_configs;
  if(!resource_configs.InitFromConfigFile("text_miner_resource.config")) {
    return -1;
  }

  TextMiner tm(&resource_configs);

  Document document;
  MockDoc(document);

  ExtractTokens(&tm, &document, std::cout);
  ExtractKeywords(&tm, &document, std::cout);
  ExtractTopics(&tm, &document, std::cout);
  ExtractTopicWords(&tm, &document, std::cout);
  ExtractCategories(&tm, &document, std::cout);

  return 0;
}

