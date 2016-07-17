// Copyright (c) 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict_manager.h"

#include "thirdparty/gflags/gflags.h"

namespace qzap {
namespace text_analysis {

DictManager::DictManager()
    : keyword_dict_(NULL),
    keyword_blacklist_(NULL),
    stopword_dict_(NULL),
    token_idf_dict_(NULL),
    word_embedding_dict_(NULL) {
}

DictManager::~DictManager() { Clear(); }

bool DictManager::Init(const std::string& dict_dir) {
  Clear();

  keyword_dict_ = new KeywordDict();
  if (!keyword_dict_->Load(dict_dir + "/dict.keyword")) {
    LOG(WARNING) << "keyword_dict initialization failed.";
    return false;
  }
  keyword_blacklist_ = new Vocabulary();
  if (!keyword_blacklist_->Load(
          dict_dir + "/dict.keyword_blacklist.vocabulary")) {
    // NOTE: 加载失败，仅打印 log
    LOG(WARNING) << "keyword_blacklist_vocabulary initialization failed.";
  }

  stopword_dict_ = new StopwordDict();
  if (!stopword_dict_->Load(dict_dir + "/dict.stopword")) {
    LOG(WARNING) << "stopword_dict initialization failed.";
    return false;
  }

  token_idf_dict_ = new TokenIdfDict();
  if (!token_idf_dict_->Load(dict_dir + "/dict.token_idf")) {
    LOG(WARNING) << "token_idf_dict initialization failed.";
    return false;
  }

  word_embedding_dict_ = new WordEmbeddingDict();
  if (!word_embedding_dict_->Load(dict_dir + "/dict.word_embedding")) {
    LOG(WARNING) << "word_embedding_dict initialization failed.";
    return false;
  }

  return true;
}

const KeywordDict* DictManager::GetKeywordDict() const {
  return keyword_dict_;
}

const Vocabulary* DictManager::GetKeywordBlacklist() const {
  return keyword_blacklist_;
}

const StopwordDict* DictManager::GetStopwordDict() const {
  return stopword_dict_;
}

const TokenIdfDict* DictManager::GetTokenIdfDict() const {
  return token_idf_dict_;
}

const WordEmbeddingDict* DictManager::GetWordEmbeddingDict() const {
  return word_embedding_dict_;
}

void DictManager::Clear() {
  if (keyword_dict_) {
    delete keyword_dict_;
    keyword_dict_ = NULL;
  }
  if (keyword_blacklist_) {
    delete keyword_blacklist_;
    keyword_blacklist_ = NULL;
  }
  if (stopword_dict_) {
    delete stopword_dict_;
    stopword_dict_ = NULL;
  }
  if (token_idf_dict_) {
    delete token_idf_dict_;
    token_idf_dict_ = NULL;
  }
  if (word_embedding_dict_) {
    delete word_embedding_dict_;
    word_embedding_dict_ = NULL;
  }
}

}  // namespace text_analysis
}  // namespace qzap

