// Copyright 2011 Tencent Inc.
// Author: Yulong Li (ulonli@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#ifndef COCKTAIL_CONTENT_ANALYZER_STRKEY_VALUE_DICT_BASE_H_
#define COCKTAIL_CONTENT_ANALYZER_STRKEY_VALUE_DICT_BASE_H_

#include <fstream>
#include <string>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/dict/dict_io.h"
#include "app/qzap/text_analysis/thirdparty/darts.h"

namespace qzap {
namespace text_analysis {

template <typename MessageType>
class StrKeyValueDictBase {
 public:
  StrKeyValueDictBase();
  virtual ~StrKeyValueDictBase();
  virtual void Clear();

  virtual bool Save(const std::string& file_name) const;
  virtual bool Load(const std::string& file_name);

  virtual const MessageType* Search(const std::string& key) const;

  struct ExtractResultType {
    std::vector<size_t> boundaries;
    const MessageType* value;
  };

  virtual bool ExtractByByte(const std::string& text,
      std::vector<ExtractResultType>* results) const;

  virtual bool ExtractByToken(const std::string& text,
      const std::vector<size_t>& token_boundaries,
      std::vector<ExtractResultType>* results) const;

 protected:
  Darts::DoubleArray dict_;
  std::vector<MessageType> value_vector_;

  DECLARE_UNCOPYABLE(StrKeyValueDictBase);
};

const size_t kMaxPrefixNumber = 128;

template <typename MessageType>
StrKeyValueDictBase<MessageType>::StrKeyValueDictBase() {}

template <typename MessageType>
StrKeyValueDictBase<MessageType>::~StrKeyValueDictBase() { Clear(); }

template <typename MessageType>
void StrKeyValueDictBase<MessageType>::Clear() {
  dict_.clear();
  value_vector_.clear();
}

template <typename MessageType>
bool StrKeyValueDictBase<MessageType>::Save(
    const std::string& file_name) const {
  std::ofstream out1(file_name.c_str());
  if (!out1) {
    LOG(ERROR) << "create file '" << file_name << "' failed!";
    return false;
  }
  size_t offset = 0;
  size_t dict_total_size = dict_.total_size();
  offset += WritePOD(dict_total_size, out1);
  out1.close();

  if (dict_.save(file_name.c_str(), "rb+", offset) != 0) {
    LOG(ERROR) << "save darts to file '" << file_name << "' failed!";
    return false;
  }
  offset += dict_total_size;

  std::fstream out2(
      file_name.c_str(),
      std::ios_base::binary | std::ios_base::in | std::ios_base::out);

  if(!out2) {
    LOG(ERROR) << "save values to file '" << file_name << "' failed!";
    return false;
  }
  out2.seekp(offset);
  WriteProtobufMessages(value_vector_.begin(), value_vector_.end(), out2);
  out2.close();

  return true;
}

template <typename MessageType>
bool StrKeyValueDictBase<MessageType>::Load(const std::string& file_name) {
  Clear();

  std::ifstream in(file_name.c_str());
  if(in.fail()) {
    LOG(ERROR) << "Open file " << file_name << " failed.";
    return false;
  }

  size_t offset = 0;
  size_t dict_total_size;
  ReadPOD(in, &dict_total_size);
  offset += sizeof(dict_total_size);

  if (dict_.open(file_name.c_str(), "rb", offset, dict_total_size) != 0) {
    LOG(ERROR) << "read darts from file '" << file_name << "' failed!";
    return false;
  }
  offset += dict_total_size;

  in.seekg(offset);
  ReadProtobufMessages(in, &value_vector_);
  in.close();

  return true;
}

template <typename MessageType>
const MessageType* StrKeyValueDictBase<MessageType>::Search(
    const std::string& key) const {
  if (dict_.size() == 0) {
    return NULL;
  }
  Darts::DoubleArray::result_pair_type result;
  dict_.exactMatchSearch<Darts::DoubleArray::result_pair_type>(key.c_str(),
                                                               result);
  if ((result.value < 0)
      || (result.value >= static_cast<int>(value_vector_.size()))) {
    return NULL;
  } else {
    return &value_vector_[result.value];
  }
}

template <typename MessageType>
bool StrKeyValueDictBase<MessageType>::ExtractByByte(
    const std::string& text,
    std::vector<ExtractResultType>* results) const {
  if (results == NULL) {
    LOG(WARNING) << "results is NULL!";
    return false;
  }
  if (dict_.size() == 0) {
    return false;
  }
  Darts::DoubleArray::result_pair_type match_result[kMaxPrefixNumber];
  for (size_t i = 0; i < text.size(); ++i) {
    if (i > 0 && isalnum(text.at(i - 1)) && isalnum(text.at(i))) {
      continue;
    }

    size_t match_count = dict_.commonPrefixSearch(
        text.c_str() + i, match_result, kMaxPrefixNumber);
    if (match_count > kMaxPrefixNumber) {
      match_count = kMaxPrefixNumber;
    }
    for (size_t j = 0; j < match_count; ++j) {
      if ((i + match_result[j].length) < text.size()
          && isalnum(text.at(i + match_result[j].length - 1))
          && isalnum(text.at(i + match_result[j].length))) {
        continue;
      }

      ExtractResultType result;
      result.boundaries.push_back(i);
      result.boundaries.push_back(i + match_result[j].length);
      result.value = &value_vector_[match_result[j].value];
      results->push_back(result);
    }
  }

  return true;
}

template <typename MessageType>
bool StrKeyValueDictBase<MessageType>::ExtractByToken(
    const std::string& text,
    const std::vector<size_t>& token_boundaries,
    std::vector<ExtractResultType>* results) const {
  if ((token_boundaries.size() == 0) || (results == NULL)) {
    return false;
  }
  if (dict_.size() == 0) {
    return false;
  }
  Darts::DoubleArray::result_pair_type match_result[kMaxPrefixNumber];
  for (size_t begin = 0; begin < (token_boundaries.size() - 1); ++begin) {
    size_t i = token_boundaries.at(begin);
    if (i > text.size()) {
      LOG(WARNING) << "token boundary error!";
      continue;
    }
    size_t match_count = dict_.commonPrefixSearch(
        text.c_str() + i, match_result, kMaxPrefixNumber);
    if (match_count > kMaxPrefixNumber) {
      match_count = kMaxPrefixNumber;
    }
    for (size_t j = 0; j < match_count; ++j) {
      // check if the boundary of current keyword candidate is legal
      ExtractResultType result;
      result.boundaries.push_back(i);
      size_t end = i + match_result[j].length;
      bool inconsistent = false;
      size_t k = (begin + 1);
      while (k < token_boundaries.size()) {
        if (token_boundaries[k] < end) {
          result.boundaries.push_back(token_boundaries[k]);
        } else if (token_boundaries[k] > end) {
          inconsistent = true;
          break;
        } else {
          result.boundaries.push_back(token_boundaries[k]);
          break;
        }
        ++k;
      }
      if (inconsistent == false) {
        result.value = &value_vector_[match_result[j].value];
        results->push_back(result);
      }
    }
  }
  return true;
}

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_STRKEY_VALUE_DICT_BASE_H_
