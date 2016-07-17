// Copyright (c) 2015-2016 Tencent Inc.
// Author: Yafei Zhang (kimmyzhang@tencent.com)
//
// 对文本行提取特征, 支持:
// 1. BoW特征
// 2. Topic特征
// 3. Keyword特征
// 4. Embedding特征

#include <cctype>
#include <fstream>  // NOLINT
#include <iostream>  // NOLINT
#include <string>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/text_miner.h"
#include "app/qzap/text_analysis/text_miner.pb.h"
#include "app/qzap/text_analysis/text_miner_resource.h"

DEFINE_bool(enable_bow, true, "Enable BoW features.");
DEFINE_bool(enable_topic, false, "Enable topic features.");
DEFINE_bool(enable_keyword, false, "Enable keyword features.");
DEFINE_bool(enable_embedding, false, "Enable embedding features.");

DEFINE_string(bow_prefix, "bow_", "BoW features prefix.");
DEFINE_string(topic_prefix, "topic_", "Topic features prefix.");
DEFINE_string(keyword_prefix, "keyword_", "Keyword features prefix.");
DEFINE_string(embedding_prefix, "embedding_", "Embedding features prefix.");

DEFINE_bool(bow_normalize_weight, true,
            "BoW features: output L2-normalized weights.");
DEFINE_bool(topic_normalize_weight, true,
            "Topic features: output L2-normalized weights.");
DEFINE_bool(keyword_normalize_weight, true,
            "Keyword features: output L2-normalized weights.");
DEFINE_bool(embedding_normalize_weight, true,
            "Embedding features: output L2-normalized weights.");

DEFINE_int32(topic_max_topic, 10, "The max number of topic features.");

DEFINE_string(separator, "\t", "Output separator.");
DEFINE_string(input_file, "-",
              "The input file. \"-\" or empty denotes the standard input.");
DEFINE_string(output_file, "-",
              "The output file. \"-\" or empty denotes the standard output.");

DECLARE_string(segmenter_data_dir);
DECLARE_string(text_miner_resource_config_file);

using namespace qzap::text_analysis;  // NOLINT

static const double EPS = 1e-6;

static bool IsSpace(const std::string& s) {
  for (size_t i = 0; i < s.size(); i++) {
    if (!std::isspace(s[i])) {
      return false;
    }
  }
  return true;
}

int main(int argc, char** argv) {
  FLAGS_segmenter_data_dir = "data/tc_data";
  FLAGS_text_miner_resource_config_file =
    "data/text_miner_resource.config.light";

  ::google::ParseCommandLineFlags(&argc, &argv, true);
  if (!FLAGS_enable_bow && !FLAGS_enable_topic) {
    LOG(WARNING) << "No features to be extracted.";
    return 0;
  }

  if (FLAGS_separator.empty()) {
    LOG(ERROR) << "Separator must be not empty.";
    return 1;
  }

  scoped_ptr<TextMinerResource> text_miner_resource;
  scoped_ptr<TextMiner> text_miner;
  text_miner_resource.reset(new TextMinerResource());
  text_miner_resource->InitFromConfigFile(
    FLAGS_text_miner_resource_config_file);
  text_miner.reset(new TextMiner(text_miner_resource.get()));

  std::istream* fin;
  if (FLAGS_input_file == "-" || FLAGS_input_file.empty()) {
    fin = &std::cin;
  } else {
    fin = new std::ifstream(FLAGS_input_file.c_str());
    if (fin->fail()) {
      LOG(ERROR) << "Open file " << FLAGS_input_file << " failed.";
      return 2;
    }
  }

  std::ostream* fout;
  if (FLAGS_output_file == "-" || FLAGS_output_file.empty()) {
    fout = &std::cout;
  } else {
    fout = new std::ofstream(FLAGS_output_file.c_str());
    if (fout->fail()) {
      LOG(ERROR) << "Open file " << FLAGS_output_file << " failed.";
      return 3;
    }
  }

  std::string line;
  while (std::getline(*fin, line)) {
    TrimString(&line);

    Document document;
    Field* field = document.add_field();
    field->set_text(line);

    std::string text;

    if (FLAGS_enable_bow) {
      text_miner->ExtractTokens(&document);
      for (int i = 0; i < document.bow_token_size(); ++i) {
        const Token& token = document.bow_token(i);
        double weight = FLAGS_bow_normalize_weight ?
                        token.weight() : token.ori_weight();
        if (weight < EPS) {
          continue;
        }
        if (token.text().empty() || IsSpace(token.text())) {
          continue;
        }
        StringAppendF(&text, "%s%s:%g%s",
                      FLAGS_bow_prefix.c_str(),
                      token.text().c_str(),
                      weight,
                      FLAGS_separator.c_str());
      }
    }

    if (FLAGS_enable_topic) {
      text_miner->InferTopics(&document);
      for (int i = 0; i < document.topic_size()
           && i < FLAGS_topic_max_topic; ++i) {
        const Topic& topic = document.topic(i);
        double weight = FLAGS_topic_normalize_weight ?
                        topic.weight() : topic.ori_weight();
        if (weight < EPS) {
          continue;
        }
        StringAppendF(&text, "%s%u:%g%s",
                      FLAGS_topic_prefix.c_str(),
                      topic.id(),
                      weight,
                      FLAGS_separator.c_str());
      }
    }

    if (FLAGS_enable_keyword) {
      text_miner->ExtractKeywords(&document);
      for (int i = 0; i < document.bow_keyword_size(); ++i) {
        const Keyword& keyword = document.bow_keyword(i);
        double weight = FLAGS_keyword_normalize_weight ?
                        keyword.weight() : keyword.ori_weight();
        if (weight < EPS) {
          continue;
        }
        StringAppendF(&text, "%s%s:%g%s",
                      FLAGS_keyword_prefix.c_str(),
                      keyword.text().c_str(),
                      weight,
                      FLAGS_separator.c_str());
      }
    }

    if (FLAGS_enable_embedding) {
      text_miner->InferEmbedding(&document);
      for (int i = 0; i < document.embedding_size(); ++i) {
        const Embedding& embedding = document.embedding(i);
        double weight = FLAGS_embedding_normalize_weight ?
                        embedding.weight() : embedding.ori_weight();
        if (weight < EPS) {
          continue;
        }
        StringAppendF(&text, "%s%d:%g%s",
                      FLAGS_embedding_prefix.c_str(),
                      i,
                      weight,
                      FLAGS_separator.c_str());
      }
    }

    if (!text.empty()) {
      text.resize(text.size() - FLAGS_separator.size());
    }
    *fout << text << std::endl;
  }

  if (fin != &std::cin) {
    delete fin;
  }
  if (fout != &std::cout) {
    delete fout;
  }

  return 0;
}
