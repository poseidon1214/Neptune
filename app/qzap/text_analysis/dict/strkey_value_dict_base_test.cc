// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/strkey_value_dict_base.h"

#include <fstream>
#include <map>
#include <string>

#include "thirdparty/gtest/gtest.h"
#include "app/qzap/common/base/string_utility.h"
#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/dict/dict.pb.h"

namespace qzap {
namespace text_analysis {

class StrKeyValueDict : public StrKeyValueDictBase<KeywordInfo> {
 public:
  StrKeyValueDict() {}
  ~StrKeyValueDict() {}

  bool Build(const std::string& path) {
    std::map<std::string, Darts::DoubleArray::value_type> key_idx_map;

    std::ifstream in_file(path.c_str());
    if (!in_file) {
      LOG(ERROR) << "Open file " << path << " failed!";
      return false;
    }

    std::string line;
    while (getline(in_file, line)) {
      TrimString(&line);
      if (line.size() == 0) { continue; }
      KeywordInfo val_info;
      value_vector_.push_back(val_info);
      key_idx_map[line] = value_vector_.size() - 1;
    }
    in_file.close();

    std::vector<const char*> key_vec;
    std::vector<Darts::DoubleArray::value_type> idx_vec;
    for (std::map<std::string, Darts::DoubleArray::value_type>::iterator it
         = key_idx_map.begin(); it != key_idx_map.end(); ++it) {
      key_vec.push_back(it->first.c_str());
      idx_vec.push_back(it->second);
    }

    if (key_vec.size() > 0) {
      int ret = dict_.build(
          key_vec.size(), &key_vec[0], NULL, &idx_vec[0]);
      if (ret != 0) {
        LOG(ERROR) << "Build keyword Double-Array failed!";
        Clear();
        return false;
      }
    }

    return true;
  }

 private:
  DECLARE_UNCOPYABLE(StrKeyValueDict);
};

TEST(StrKeyValueDict, Build) {
  StrKeyValueDict strkey_value_dict;

  EXPECT_TRUE(strkey_value_dict.Build("testdata/keywords_test.dat"));

  const KeywordInfo* value =
      strkey_value_dict.Search("什么射击网络游戏好玩");
  EXPECT_TRUE(NULL != value);
  value = strkey_value_dict.Search("Nokia手机");
  EXPECT_TRUE(NULL != value);
  value = strkey_value_dict.Search("");
  EXPECT_TRUE(NULL == value);
}

TEST(StrKeyValueDict, ExtractByByte) {
  std::string test_text = "请问贺州哪儿牙齿美容的价格最便宜";
  std::vector<StrKeyValueDict::ExtractResultType> result_vec;
  StrKeyValueDict strkey_value_dict;
  EXPECT_TRUE(strkey_value_dict.Build("testdata/keywords_test.dat"));

  EXPECT_TRUE(strkey_value_dict.ExtractByByte(test_text, &result_vec));

  EXPECT_EQ(3U, result_vec.size());
  EXPECT_EQ(test_text.substr(
          result_vec[0].boundaries[0],
          result_vec[0].boundaries.back()
          - result_vec[0].boundaries[0]), "贺州");
  EXPECT_EQ(
      test_text.substr(result_vec[1].boundaries[0],
                       result_vec[1].boundaries.back()
                       - result_vec[1].boundaries[0]), "牙齿美容");
  EXPECT_EQ(
      test_text.substr(result_vec[2].boundaries[0],
                       result_vec[2].boundaries.back()
                       - result_vec[2].boundaries[0]), "牙齿美容的价格");

  result_vec.clear();
  EXPECT_TRUE(strkey_value_dict.ExtractByByte("", &result_vec));
  EXPECT_EQ((int)(result_vec.size()), 0);
}

TEST(StrKeyValueDict, ExtractByToken) {
  std::string test_text = "请问贺州哪儿牙齿美容的价格最便宜";
  std::vector<StrKeyValueDict::ExtractResultType> result_vec;
  std::vector<size_t> token_boundaries;
  token_boundaries.push_back(0);
  token_boundaries.push_back(6);
  token_boundaries.push_back(12);
  token_boundaries.push_back(18);
  token_boundaries.push_back(24);
  token_boundaries.push_back(30);
  token_boundaries.push_back(33);
  token_boundaries.push_back(39);
  token_boundaries.push_back(45);
  token_boundaries.push_back(48);
  token_boundaries.push_back(54);
  std::vector<StrKeyValueDict::ExtractResultType> results;
  StrKeyValueDict strkey_value_dict;
  EXPECT_TRUE(strkey_value_dict.Build("testdata/keywords_test.dat"));

  EXPECT_TRUE(strkey_value_dict.ExtractByToken(test_text, token_boundaries,
                                               &results));

  EXPECT_EQ(3U, results.size());
  const std::vector<size_t>& b0 = results[0].boundaries;
  const std::vector<size_t>& b1 = results[1].boundaries;
  const std::vector<size_t>& b2 = results[2].boundaries;
  EXPECT_EQ(test_text.substr(b0[0], b0.back() - b0[0]), "贺州");
  EXPECT_EQ(test_text.substr(b1[0], b1.back() - b1[0]), "牙齿美容");
  EXPECT_EQ(test_text.substr(b2[0], b2.back() - b2[0]), "牙齿美容的价格");
}

}  // namespace text_analysis
}  // namespace qzap

