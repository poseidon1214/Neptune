// Copyright (c) 2014 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//
// TextMiner抽取文本特征tool

#include "app/qzap/text_analysis/text_miner.h"

#include <fstream>

#include "common/base/string/algorithm.h"
#include "common/base/string/string_number.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/text_analysis/text_miner_resource.h"

DEFINE_bool(output_tokens, true, "extract tokens and output");
DEFINE_bool(output_keywords, true, "extract keywords and output");
DEFINE_bool(output_topics, true, "extract topics and output");
DEFINE_bool(output_topic_words, true, "extract topic words and output");
DEFINE_bool(output_categories, true, "extract categories and output");
DEFINE_string(document_field_separator, "\t",
              "input document field separator");
DEFINE_string(document_field_weights, "1",
              "input document field weights, separated by ':'");
DEFINE_string(feature_separator, "\t", "separator of different feature ids");
DEFINE_string(weight_separator, " ", "separator of feature id and weight");
DEFINE_string(text_file, "", "input text file, one doc per line");
DEFINE_string(feature_file_prefix, "",
              "output feature file prefix, one doc per line");
DECLARE_string(text_miner_resource_config_file);

namespace qzap {
namespace text_analysis {

bool ParseDocumentFieldWeights(const std::string& s,
                               std::vector<double>* weights) {
  std::vector<std::string> fields;
  SplitStringAndStrip(s, ":", &fields);
  weights->resize(fields.size());
  for (size_t i = 0; i < fields.size(); ++i) {
    if (!ParseNumber(fields[i], &(*weights)[i], NULL)) {
      return false;
    }
  }
  return true;
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

  std::vector<double> field_weights;
  if (!ParseDocumentFieldWeights(FLAGS_document_field_weights,
                                 &field_weights)) {
    LOG(FATAL) << "Failed to parse --document_field_weights: "
        << FLAGS_document_field_weights;
    return -1;
  }

  std::ifstream is(FLAGS_text_file.c_str());
  if (!is) {
    LOG(FATAL) << "Failed to open --text_file: " << FLAGS_text_file;
    return -1;
  }
  std::ofstream os_tokens, os_keywords, os_topics,
      os_topic_words, os_categories;
  if (FLAGS_output_tokens) {
    os_tokens.open((FLAGS_feature_file_prefix + ".tokens").c_str());
  }
  if (FLAGS_output_keywords) {
    os_keywords.open((FLAGS_feature_file_prefix + ".keywords").c_str());
  }
  if (FLAGS_output_topics) {
    os_topics.open((FLAGS_feature_file_prefix + ".topics").c_str());
  }
  if (FLAGS_output_topic_words) {
    os_topic_words.open((FLAGS_feature_file_prefix + ".topic_words").c_str());
  }
  if (FLAGS_output_categories) {
    os_categories.open((FLAGS_feature_file_prefix + ".categories").c_str());
  }

  scoped_ptr<TextMinerResource> text_miner_resource(new TextMinerResource());
  text_miner_resource->InitFromConfigFile(
      FLAGS_text_miner_resource_config_file);
  scoped_ptr<TextMiner> text_miner(new TextMiner(text_miner_resource.get()));
  LOG(INFO) << "Succeeded to initialize textminer";

  std::string line;
  int64_t count = 0;
  while (std::getline(is, line, '\n')) {
    ++count;
    std::vector<std::string> document_fields;
    SplitStringKeepEmpty(line, FLAGS_document_field_separator, &document_fields);

    // 保证输出feature文件行数同输入text文件行数一致
    Document document;
    if (document_fields.size() != field_weights.size()) {
      LOG(ERROR) << "#document_fields(" << document_fields.size()
          << ") != #field_weights(" << field_weights.size()
          << "), line " << count << ": " << line;
    } else {
      for (size_t i = 0; i < document_fields.size(); ++i) {
        Field* field = document.add_field();
        field->set_type(UNKNOWN_FIELD);
        field->set_weight(field_weights[i]);
        field->set_text(document_fields[i]);
      }
    }

    if (FLAGS_output_tokens &&
        !ExtractTokens(text_miner.get(), &document, os_tokens)) {
      LOG(ERROR) << "Failed to extract tokens, line " << count;
    }
    if (FLAGS_output_keywords &&
        !ExtractKeywords(text_miner.get(), &document, os_keywords)) {
      LOG(ERROR) << "Failed to extract keywords, line " << count;
    }
    if (FLAGS_output_topics &&
        !ExtractTopics(text_miner.get(), &document, os_topics)) {
      LOG(ERROR) << "Failed to extract topics, line " << count;
    }
    if (FLAGS_output_topic_words &&
        !ExtractTopicWords(text_miner.get(), &document, os_topic_words)) {
      LOG(ERROR) << "Failed to extract topic words, line " << count;
    }
    if (FLAGS_output_categories &&
        !ExtractCategories(text_miner.get(), &document, os_categories)) {
      LOG(ERROR) << "Failed to extract categories, line " << count;
    }
  }

  LOG(INFO) << "line count: " << count;
  return 0;
}

}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  return qzap::text_analysis::main(argc, argv);
}
