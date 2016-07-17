// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/thirdparty/darts.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <string>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace qzap {
namespace text_analysis {

const std::size_t kValidKeysNum = 1 << 16;
const std::size_t kInvalidKeysNum = 1 << 17;

class DartsTest : public ::testing::Test {
 public:
  DartsTest() {}
  virtual ~DartsTest() {}

  void GenerateValidKeys(std::size_t num_keys,
                         std::set<std::string> *valid_keys) {
    std::vector<char> key;
    while (valid_keys->size() < num_keys) {
      key.resize(1 + (rand() % 8));
      for (std::size_t i = 0; i < key.size(); ++i) {
        key[i] = 'A' + (rand() % 26);
      }
      valid_keys->insert(std::string(&key[0], key.size()));
    }
  }

  void GenerateInvalidKeys(std::size_t num_keys,
                           const std::set<std::string> &valid_keys,
                           std::set<std::string> *invalid_keys) {
    std::vector<char> key;
    while (invalid_keys->size() < num_keys) {
      key.resize(1 + (rand() % 8));
      for (std::size_t i = 0; i < key.size(); ++i) {
        key[i] = 'A' + (rand() % 26);
      }
      std::string generated_key(&key[0], key.size());
      if (valid_keys.find(generated_key) == valid_keys.end()) {
        invalid_keys->insert(std::string(&key[0], key.size()));
      }
    }
  }

  virtual void SetUp() {
    srand(static_cast<unsigned int>(time(NULL)));
    GenerateValidKeys(kValidKeysNum, &valid_keys);
    GenerateInvalidKeys(kInvalidKeysNum, valid_keys, &invalid_keys);

    for (std::set<std::string>::const_iterator it = valid_keys.begin();
         it != valid_keys.end();
         ++it) {
      keys_.push_back(it->c_str());
      lengths_.push_back(it->length());
      values_.push_back(
          static_cast<Darts::DoubleArray::value_type>(keys_.size() - 1));
    }
    dic_.build(keys_.size(), &keys_[0], &lengths_[0], &values_[0]);
  }

  virtual void TearDown() {}

 protected:
  std::set<std::string> valid_keys;
  std::set<std::string> invalid_keys;

  std::vector<const char *> keys_;
  std::vector<std::size_t> lengths_;
  std::vector<Darts::DoubleArray::value_type> values_;

  Darts::DoubleArray dic_;
};

TEST_F(DartsTest, SaveAndLoad) {
  std::string filename = "test.dic";
  EXPECT_TRUE(0 == dic_.save(filename.c_str()));
  EXPECT_TRUE(0 == dic_.open(filename.c_str()));

  int offset = 10;
  EXPECT_TRUE(0 == dic_.save(filename.c_str(), "wb+", offset));
  EXPECT_TRUE(0 == dic_.open(filename.c_str(), "rb", offset));
}

TEST_F(DartsTest, CommonPrefixSearch) {
  static const std::size_t kMaxResultsNum = 16;
  Darts::DoubleArray::result_pair_type results[kMaxResultsNum];
  Darts::DoubleArray::result_pair_type results_with_length[kMaxResultsNum];

  for (std::size_t i = 0; i < keys_.size(); ++i) {
    std::size_t num_results = dic_.commonPrefixSearch(
        keys_[i], results, kMaxResultsNum);

    EXPECT_TRUE(num_results >= 1);
    EXPECT_TRUE(num_results < kMaxResultsNum + 1);

    EXPECT_TRUE(results[num_results - 1].value == values_[i]);
    EXPECT_TRUE(results[num_results - 1].length == lengths_[i]);

    std::size_t num_results_with_length = dic_.commonPrefixSearch(
        keys_[i], results_with_length, kMaxResultsNum, lengths_[i]);

    EXPECT_TRUE(num_results == num_results_with_length);
    for (std::size_t j = 0; j < num_results; ++j) {
      EXPECT_TRUE(results[j].value == results_with_length[j].value);
      EXPECT_TRUE(results[j].length == results_with_length[j].length);
    }
  }

  for (std::set<std::string>::const_iterator it = invalid_keys.begin();
       it != invalid_keys.end(); ++it) {
    std::size_t num_results = dic_.commonPrefixSearch(
        it->c_str(), results, kMaxResultsNum);

    EXPECT_TRUE(num_results < kMaxResultsNum + 1);
    if (num_results > 0) {
      EXPECT_TRUE(results[num_results - 1].value != -1);
      EXPECT_TRUE(results[num_results - 1].length < it->length());
    }

    std::size_t num_results_with_length = dic_.commonPrefixSearch(
        it->c_str(), results_with_length, kMaxResultsNum, it->length());

    EXPECT_TRUE(num_results == num_results_with_length);
    for (std::size_t j = 0; j < num_results; ++j) {
      EXPECT_TRUE(results[j].value == results_with_length[j].value);
      EXPECT_TRUE(results[j].length == results_with_length[j].length);
    }
  }
}

TEST_F(DartsTest, Traverse) {
  for (std::size_t i = 0; i < keys_.size(); ++i) {
    const char *key = keys_[i];
    std::size_t id = 0;
    std::size_t key_pos = 0;
    Darts::DoubleArray::value_type result = 0;
    for (std::size_t j = 0; j < lengths_[i]; ++j) {
      result = dic_.traverse(key, id, key_pos, j + 1);
      EXPECT_TRUE(result != -2);
    }
    EXPECT_TRUE(result == values_[i]);
  }

  for (std::set<std::string>::const_iterator it = invalid_keys.begin();
       it != invalid_keys.end(); ++it) {
    const char *key = it->c_str();
    std::size_t id = 0;
    std::size_t key_pos = 0;
    Darts::DoubleArray::value_type result = 0;
    for (std::size_t i = 0; i < it->length(); ++i) {
      result = dic_.traverse(key, id, key_pos, i + 1);
      if (result == -2) {
        break;
      }
    }
    EXPECT_TRUE(result < 0);
  }
}

}  // namespace text_analysis
}  // namespace qzap
