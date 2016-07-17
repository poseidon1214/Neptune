// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/keyword/keyword_extractor.h"

#include <math.h>
#include <algorithm>
#include <string>
#include <tr1/unordered_map>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/hash.h"
#include "app/qzap/text_analysis/dict/dict.pb.h"
#include "app/qzap/text_analysis/dict/keyword_dict.h"
#include "app/qzap/text_analysis/dict/stopword_dict.h"
#include "app/qzap/text_analysis/dict/vocabulary.h"
#include "app/qzap/text_analysis/text_miner.pb.h"

namespace qzap {
namespace text_analysis {

KeywordExtractor::KeywordExtractor(const DictManager& dict_manager)
    : dict_manager_(dict_manager) {}

bool KeywordExtractor::Extract(Document* document) const {
  if (!document->has_segmented()) {
    LOG(WARNING) << "The document must be segmeted first.";
    return false;
  }

  if (!ExtractDocumentKeywords(document)) {
    return false;
  }
  CalcWeight(document);
  document->set_has_extracted_keyword(true);

  return true;
}

bool KeywordExtractor::ExtractDocumentKeywords(Document* document) const {
  const KeywordDict* keyword_dict = dict_manager_.GetKeywordDict();
  if (keyword_dict == NULL) {
    LOG(ERROR) << "keyword_dict is NULL.";
    return false;
  }

  BowKeywordsMap bow_keywords_map;
  std::vector<size_t> token_boundaries;
  std::tr1::unordered_map<size_t, int> offset_to_token;
  std::vector<KeywordDict::ExtractResultType> results;
  for (int i = 0; i < document->field_size(); ++i) {
    const Field& field = document->field(i);
    // 匹配抽取每个Filed包含的Keyword
    if (field.token_size() == 0) { continue; }

    std::string text;  // 拼接出新的field_text
    token_boundaries.clear();
    offset_to_token.clear();
    token_boundaries.push_back(0);
    for (int j = 0; j < field.token_size(); ++j) {
      const TokenOccurence& token = field.token(j);
      StringAppendF(&text, "%s", token.text().c_str());
      token_boundaries.push_back(token.offset() + token.text().length());
      offset_to_token[token.offset()] = j;
    }

    results.clear();
    keyword_dict->ExtractByToken(text, token_boundaries, &results);

    for (size_t j = 0; j < results.size(); ++j) {
      std::string keyword_text = text.substr(
          results[j].boundaries[0],
          results[j].boundaries.back() - results[j].boundaries[0]);
      // 是否出现在黑名单词典或停用词词典中
      if (!IsValid(keyword_text)) {
        continue;
      }

      BowKeywordsMapIter iter = bow_keywords_map.find(keyword_text);

      Keyword* bow_keyword = NULL;
      if (iter == bow_keywords_map.end()) {
        bow_keyword = document->add_bow_keyword();
        bow_keyword->set_text(keyword_text);
        bow_keyword->set_signature(hash_string(bow_keyword->text()));
        int begin = offset_to_token[results[j].boundaries[0]];
        for (size_t k = 0; k < results[j].boundaries.size() - 1; ++k) {
          bow_keyword->add_token(field.token(begin + k).text());
        }
        bow_keyword->set_weight(field.weight());  // 暂时仅记录 field weight

        bow_keywords_map[keyword_text] = bow_keyword;
      } else {
        bow_keyword = iter->second;
        bow_keyword->set_weight(bow_keyword->weight() + field.weight());
      }
    }
  }

  return true;
}

bool KeywordExtractor::IsValid(const std::string& keyword_text) const {
  const Vocabulary* keyword_blacklist = dict_manager_.GetKeywordBlacklist();
  const StopwordDict* stopword_dict = dict_manager_.GetStopwordDict();

  return !((keyword_blacklist != NULL
            && keyword_blacklist->HasWord(keyword_text))
           || (stopword_dict != NULL
              && stopword_dict->IsStopword(keyword_text)));
}

void KeywordExtractor::CalcWeight(Document* document) const {
  if (document->bow_keyword_size() == 0) { return; }

  const KeywordDict* keyword_dict = dict_manager_.GetKeywordDict();

  // bow_keyword's weight = sum(field_weight(f) * frequency(f)) * static_weight,
  // f stand for the field index
  double sum = 0.0;
  for (int i = 0; i < document->bow_keyword_size(); ++i) {
    Keyword* bow_keyword = document->mutable_bow_keyword(i);
    const KeywordInfo* keyword_info = keyword_dict->Search(bow_keyword->text());
    double weight = bow_keyword->weight() * keyword_info->weight();
    bow_keyword->set_ori_weight(weight);
    sum += weight * weight;
  }
  sum = sqrt(sum);

  // L2-Normalize
  static const double kEpsilon = 1E-6;
  if (sum < kEpsilon) {
    return;
  }
  for (int i = 0; i < document->bow_keyword_size(); ++i) {
    Keyword* bow_keyword = document->mutable_bow_keyword(i);
    double weight = bow_keyword->ori_weight() / sum;
    bow_keyword->set_weight(weight);
  }
}

}  // namespace text_analysis
}  // namespace qzap

