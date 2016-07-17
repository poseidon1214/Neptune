// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/vocabulary.h"

#include <stdint.h>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/text_analysis/dict/dict_io.h"

DEFINE_bool(using_default_index, true,
            "whether to need to given a default index for all of the words,"
            "default true");

namespace qzap {
namespace text_analysis {

const size_t kMaxTokenLength = 128;

Vocabulary::Vocabulary() {}

Vocabulary::~Vocabulary() { Clear(); }

void Vocabulary::Clear() {
  dict_.clear();
  index_to_word_.clear();
}

bool Vocabulary::Build(const std::string& filename) {
  Clear();

  // load data from files
  std::map<std::string, uint32_t> word_to_index;
  std::ifstream fs;
  fs.open(filename.c_str(), std::ios_base::in);
  if (fs.fail()) {
    LOG(ERROR) << "Open file " << filename << " failed.";
    return false;
  }

  std::string line;
  while (std::getline(fs, line)) {
    TrimString(&line);
    if (line.length() > 0) {
      std::vector<std::string> field_vec;
      SplitString(line, "\t", &field_vec);
      if (FLAGS_using_default_index) {
        index_to_word_.push_back(field_vec[0]);
        word_to_index[field_vec[0]] = index_to_word_.size() - 1;
      } else {
        uint32_t index;
        if (field_vec.size() > 1 && StringToNumeric(field_vec[1], &index)) {
          while (index_to_word_.size() <= index) {
            index_to_word_.push_back("");
          }
          index_to_word_[index] = field_vec[0];
          word_to_index[field_vec[0]] = index;
        } else {
          LOG(WARNING) << "line format error, line: " << line;
        }
      }
    }
  }
  fs.close();

  // build vocabulary based on darts
  std::vector<const char*> word_vec;
  std::vector<Darts::DoubleArray::value_type> index_vec;
  for (std::map<std::string, uint32_t>::iterator iter = word_to_index.begin();
       iter != word_to_index.end(); ++iter) {
    word_vec.push_back((iter->first).c_str());
    index_vec.push_back(iter->second);
  }
  int ret = dict_.build(word_vec.size(), &word_vec[0], NULL, &index_vec[0]);
  if (ret != 0) {
    LOG(ERROR) << "Build vocabulary failed.";
    Clear();
    return false;
  }
  return true;
}

bool Vocabulary::Save(const std::string& filename) const {
  std::ofstream out(filename.c_str());
  if (out.fail()) {
    LOG(ERROR) << "Create file '" << filename << "' failed.";
    return false;
  }
  size_t offset = 0;
  size_t dict_total_size = dict_.total_size();
  offset += WritePOD(dict_total_size, out);

  if (dict_.save(filename.c_str(), "rb+", offset) != 0) {
    LOG(ERROR) << "Save darts to file '" << filename << "' failed.";
    return false;
  }
  offset += dict_total_size;

  out.seekp(offset);
  offset += WritePOD(index_to_word_.size(), out);
  for (size_t i = 0; i < index_to_word_.size(); ++i) {
    offset += WriteString(index_to_word_[i], out);
  }
  out.close();

  return true;
}

bool Vocabulary::Load(const std::string& filename) {
  Clear();

  std::ifstream in(filename.c_str());
  if (in.fail()) {
    LOG(ERROR) << "Open file " << filename << " failed.";
    return false;
  }
  size_t offset = 0;
  size_t dict_total_size;
  ReadPOD(in, &dict_total_size);
  offset += sizeof(dict_total_size);

  if (dict_.open(filename.c_str(), "rb", offset, dict_total_size) != 0) {
    LOG(ERROR) << "Read darts from file '" << filename << "' failed.";
    return false;
  }
  offset += dict_total_size;

  in.seekg(offset);

  size_t index_to_word_size;
  ReadPOD(in, &index_to_word_size);
  for (size_t i = 0; i < index_to_word_size; ++i) {
    std::string word;
    offset += ReadString(in, &word);
    index_to_word_.push_back(word);
  }

  in.close();
  return true;
}

bool Vocabulary::HasWord(const std::string& word) const {
  return (WordIndex(word) != -1);
}

int Vocabulary::WordIndex(const std::string& word) const {
  if (dict_.size() == 0) {
    return -1;
  }

  Darts::DoubleArray::value_type value;
  dict_.exactMatchSearch(word.c_str(), value);
  return value;
}

const std::string& Vocabulary::Word(int index) const {
  CHECK(index >= 0 && index < static_cast<int>(index_to_word_.size()))
      << "The input index '" << index << "' is OutOfBound.";
  return index_to_word_[index];
}

int Vocabulary::Size() const {
  return index_to_word_.size();
}

bool Vocabulary::ExtractByByte(const std::string& text,
                               std::vector<ExtractResultType>* results) const {
  CHECK(results != NULL) << "The parameter 'results' is NULL.";
  results->clear();
  CHECK_LT(0, dict_.size()) << "The vocabulary is NULL.";

  Darts::DoubleArray::result_pair_type match_result[kMaxTokenLength];
  for (size_t i = 0; i < text.size(); ++i) {
    // 保证连续的英文串不被切分
    if ((i > 0) && (isalnum(text.at(i - 1))) && (isalnum(text.at(i)))) {
      continue;
    }

    size_t match_count = dict_.commonPrefixSearch(text.c_str() + i,
                                                  match_result,
                                                  kMaxTokenLength);
    int match_number
        = ((match_count < kMaxTokenLength) ? match_count : kMaxTokenLength);
    for (int j = 0; j < match_number; ++j) {
      if (match_result[j].value < 0 || match_result[j].length <= 0) {
        continue;
      }
      if (((i + match_result[j].length) < text.size())
          && (isalnum(text.at(i + match_result[j].length - 1)))
          && (isalnum(text.at(i + match_result[j].length)))) {
        continue;
      }

      ExtractResultType result;
      result.boundaries.push_back(i);
      result.boundaries.push_back(i + match_result[j].length);
      results->push_back(result);
    }
  }

  return true;
}

bool Vocabulary::ExtractByToken(const std::string& text,
                                const std::vector<size_t>& token_boundaries,
                                std::vector<ExtractResultType>* results) const {
  CHECK(results != NULL) << "The parameter 'results' is NULL.";
  results->clear();
  CHECK_LT(0, dict_.size()) << "The vocabulary is NULL.";

  Darts::DoubleArray::result_pair_type match_result[kMaxTokenLength];
  for (size_t begin = 0; begin < (token_boundaries.size() - 1); ++begin) {
    size_t i = token_boundaries.at(begin);
    CHECK_LT(i, text.size()) << "Token boundary error!";

    size_t match_count = dict_.commonPrefixSearch(text.c_str() + i,
                                                  match_result,
                                                  kMaxTokenLength);
    int match_number
        = ((match_count < kMaxTokenLength) ? match_count : kMaxTokenLength);
    for (int j = 0; j < match_number; ++j) {
      if (match_result[j].value < 0 || match_result[j].length <= 0) {
        continue;
      }
      // check if the boundary of the matched keyword candidate is legal
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
        results->push_back(result);
      }
    }
  }

  return true;
}

}  // namespace text_analysis
}  // namespace qzap
