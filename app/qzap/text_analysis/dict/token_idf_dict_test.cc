// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/token_idf_dict.h"

#include "thirdparty/gtest/gtest.h"

namespace qzap {
namespace text_analysis {

const float kEps = 1E-6;

TEST(TokenIdfDictTest, Build) {
  std::string commercial_file = "./testdata/idf/bidwords.idf";
  std::string general_file = "./testdata/idf/soso_query_log_idf.dat";

  TokenIdfDict token_idf_dict1;
  EXPECT_TRUE(token_idf_dict1.Build(commercial_file, general_file));

  std::string invalid_commercial_file = "invalid_soso_query_log_idf.dat";
  std::string invlid_general_file = "invalid_soso.idf";
  TokenIdfDict token_idf_dict2;
  EXPECT_FALSE(token_idf_dict2.Build(invalid_commercial_file,
                                   invlid_general_file));
}

TEST(TokenIdfDictTest, IdfInfo) {
  std::string commercial_file = "./testdata/idf/bidwords.idf";
  std::string general_file = "./testdata/idf/soso_query_log_idf.dat";
  TokenIdfDict token_idf_dict;
  token_idf_dict.Build(commercial_file, general_file);

  const IdfInfo* idf_info = token_idf_dict.Search("网站");
  EXPECT_TRUE(idf_info != NULL);
  EXPECT_TRUE(idf_info->has_idf_commercial());
  EXPECT_NEAR(idf_info->idf_commercial(), 3.89436928557, kEps);

  EXPECT_TRUE(idf_info->has_idf_general());
  EXPECT_NEAR(idf_info->idf_general(), 5.56092882289, kEps);

  idf_info = token_idf_dict.Search("酒店");
  EXPECT_TRUE(idf_info->has_idf_commercial());
  EXPECT_NEAR(idf_info->idf_commercial(), 2.27827488397, kEps);
  EXPECT_FALSE(idf_info->has_idf_general());

  EXPECT_TRUE(token_idf_dict.Search("那么") == NULL);
}

TEST(TokenIdfDict, SaveAndLoad) {
  std::string commercial_file = "./testdata/idf/bidwords.idf";
  std::string general_file = "./testdata/idf/soso_query_log_idf.dat";
  TokenIdfDict token_idf_dict1;
  token_idf_dict1.Build(commercial_file, general_file);
  EXPECT_TRUE(token_idf_dict1.Save("./testdata/dict.token_idf_dict"));
  TokenIdfDict token_idf_dict2;
  EXPECT_TRUE(token_idf_dict2.Load("./testdata/dict.token_idf_dict"));

  const IdfInfo* idf_info = token_idf_dict2.Search("网站");
  EXPECT_TRUE(idf_info != NULL);
  EXPECT_TRUE(idf_info->has_idf_commercial());
  EXPECT_NEAR(idf_info->idf_commercial(), 3.89436928557, kEps);

  EXPECT_TRUE(idf_info->has_idf_general());
  EXPECT_NEAR(idf_info->idf_general(), 5.56092882289, kEps);

  idf_info = token_idf_dict2.Search("酒店");
  EXPECT_TRUE(idf_info->has_idf_commercial());
  EXPECT_NEAR(idf_info->idf_commercial(), 2.27827488397, kEps);
  EXPECT_FALSE(idf_info->has_idf_general());

  EXPECT_TRUE(token_idf_dict2.Search("那么") == NULL);
}

}  // namespace text_analysis
}  // namespace qzap
