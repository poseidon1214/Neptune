// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/token/token_extractor.h"

#include <math.h>
#include <stdint.h>
#include <string>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include "thirdparty/gflags/gflags.h"
#include "app/qzap/common/utility/hash.h"
#include "app/qzap/text_analysis/dict/token_idf_dict.h"
#include "app/qzap/text_analysis/dict_manager.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

DEFINE_bool(extract_keytokens_only, true,
            "whether to filter unimportant tokens,"
            "such as stopwords and function words");
DEFINE_double(commercial_idf_factor, 0.6, "the factor of commercial_idf");

namespace qzap {
namespace text_analysis {

static const double kEpsilon = 1E-6;

TokenExtractor::TokenExtractor(const DictManager& dict_manager)
    : dict_manager_(dict_manager) {}

TokenExtractor::~TokenExtractor() {}

bool TokenExtractor::Extract(Document* document) const {
  if (!document->has_segmented()) {
    LOG(ERROR) << "The document must be segmeted first.";
    return false;
  }

  const TokenIdfDict* token_idf_dict = dict_manager_.GetTokenIdfDict();
  if (token_idf_dict == NULL) {
    LOG(ERROR) << "token_idf_dict is NULL.";
    return false;
  }

  BowTokensMap bow_tokens_map;
  for (int i = 0; i < document->field_size(); ++i) {
    const Field& field = document->field(i);
    for (int j = 0; j < field.token_size(); ++j) {
      if (!FLAGS_extract_keytokens_only || !IsFilter(field.token(j))) {
        AddBowToken(field,
                    field.token(j),
                    token_idf_dict,
                    document,
                    &bow_tokens_map);
      }
    }
  }
  document->set_has_extracted_token(true);

  // bow_token's weight = sum(field_weight(f) * tf(f)) * idf,
  // f stand for the field index
  double sum = 0.0;
  for (int i = 0; i < document->bow_token_size(); ++i) {
    Token* bow_token = document->mutable_bow_token(i);
    double weight = bow_token->weight() * bow_token->idf();
    bow_token->set_ori_weight(weight);
    sum += weight * weight;
  }
  sum = sqrt(sum);

  // L2-Normalize
  if (sum < kEpsilon) {
    return true;
  }
  for (int i = 0; i < document->bow_token_size(); ++i) {
    Token* bow_token = document->mutable_bow_token(i);
    bow_token->set_weight(bow_token->ori_weight() / sum);
  }

  return true;
}

void TokenExtractor::AddBowToken(const Field& field,
                                 const TokenOccurence& token,
                                 const TokenIdfDict* token_idf_dict,
                                 Document* document,
                                 BowTokensMap* bow_tokens_map) const {
  BowTokensMapIter iter = bow_tokens_map->find(token.text());
  Token* bow_token = NULL;
  if (iter == bow_tokens_map->end()) {
    bow_token = document->add_bow_token();
    bow_token->set_text(token.text());
    bow_token->set_signature(hash_string(token.text()));
    bow_token->set_weight(field.weight());
    bow_token->set_idf(GetIdf(*token_idf_dict, token.text()));
    bow_token->set_tf(1);

    (*bow_tokens_map)[token.text()] = bow_token;
  } else {
    bow_token = iter->second;
    bow_token->set_tf(bow_token->tf() + 1);
    bow_token->set_weight(bow_token->weight() + field.weight());
  }
}

double TokenExtractor::GetIdf(const TokenIdfDict& token_idf_dict,
                              const std::string& word) const {
  const IdfInfo* idf_info = token_idf_dict.Search(word);
  if (idf_info == NULL) {
    return 0.0;
  }

  double idf = (1.0 - FLAGS_commercial_idf_factor) * idf_info->idf_general()
      + FLAGS_commercial_idf_factor * idf_info->idf_commercial();
  return idf;
}

// 待过滤的词性列表
static const std::string kStopWordTypes[] = {
  "c",  // 连词
  "e",  // 叹词
  "h",  // 前接成分
  "k",  // 后接成分
  "m",  // 数词
  "mg", // 数词性语素
  "o",  // 拟声词
  "p",  // 介词
  "r",  // 代词
  "rg",  // 代词性语素
  "u",  // 助词
  "w",  // 标点符号
  "x",  // 非语素字
  "y",  // 语气词
  "yg",  // 语气词语素
  "www",  // URL
  "tele",  // 电话号码
  "email"  // email
};

static const std::tr1::unordered_set<std::string> kStopWordTypeSet(
    kStopWordTypes,
    kStopWordTypes + sizeof(kStopWordTypes) / sizeof(kStopWordTypes[0]));

bool TokenExtractor::IsFilter(const TokenOccurence& token) const {
  return (token.is_stopword() ||
          kStopWordTypeSet.find(token.word_type()) != kStopWordTypeSet.end());
}

}  // namespace text_analysis
}  // namespace qzap

