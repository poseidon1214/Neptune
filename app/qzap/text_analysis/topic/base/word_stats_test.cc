// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/base/word_stats.h"

#include <fstream>

#include "app/qzap/common/base/string_utility.h"
#include "thirdparty/gtest/gtest.h"

namespace qzap {
namespace text_analysis {
namespace base {

using std::tr1::unordered_map;

const int32_t kNumWords = 5;

TEST(WordStatsTest, Empty) {
  WordStats ws(3);
  EXPECT_EQ(ws.NumTopics(), 3);
  EXPECT_EQ(ws.NumWords(), 0);
  EXPECT_TRUE(!ws.HasWord(0));
}

TEST(WordStatsTest, Parse) {
  DenseTopicHistogram h;
  h.Resize(3);
  h[1] = 3;
  h[2] = 5;

  unordered_map<int32_t, DenseTopicHistogram> ws_map;
  for (int32_t w = 100; w < 100 + kNumWords; ++w) {
    ws_map[w] = h;
  }

  WordStats ws(3);
  ws.ParseFrom(ws_map);

  EXPECT_EQ(ws.NumTopics(), 3);
  EXPECT_EQ(ws.NumWords(), kNumWords);

  EXPECT_EQ(ws.HasWord(0), false);
  EXPECT_EQ(ws.HasWord(99), false);
  EXPECT_EQ(ws.HasWord(100), true);
  EXPECT_EQ(ws.HasWord(104), true);
  EXPECT_EQ(ws.HasWord(105), false);
}

TEST(WordStatsTest, SaveLoad) {
  DenseTopicHistogram h;
  h.Resize(3);
  h[1] = 3;
  h[2] = 5;

  unordered_map<int32_t, DenseTopicHistogram> ws_map;
  for (int32_t w = 100; w < 100 + kNumWords; ++w) {
    ws_map[w] = h;
  }

  WordStats ws(3);
  ws.ParseFrom(ws_map);

  std::string filename = "test.word_stats";
  std::ofstream fout(filename.c_str());
  EXPECT_FALSE(fout.fail());
  EXPECT_TRUE(ws.Save(&fout));
  fout.close();

  std::ifstream fin(filename.c_str());
  EXPECT_FALSE(fin.fail());
  EXPECT_TRUE(ws.Load(&fin, true));
  fin.close();

  EXPECT_EQ(ws.NumTopics(), 3);
  EXPECT_EQ(ws.NumWords(), kNumWords);

  EXPECT_EQ(ws.HasWord(0), false);
  EXPECT_EQ(ws.HasWord(99), false);
  EXPECT_EQ(ws.HasWord(100), true);
  EXPECT_EQ(ws.HasWord(104), true);
  EXPECT_EQ(ws.HasWord(105), false);

  EXPECT_TRUE(ws.LoadAndMerge(std::vector<std::string>(1, filename)));

  EXPECT_EQ(ws.NumTopics(), 3);
  EXPECT_EQ(ws.NumWords(), kNumWords);

  EXPECT_EQ(ws.HasWord(0), false);
  EXPECT_EQ(ws.HasWord(99), false);
  EXPECT_EQ(ws.HasWord(100), true);
  EXPECT_EQ(ws.HasWord(104), true);
  EXPECT_EQ(ws.HasWord(105), false);
}

TEST(WordStatsTest, Iterator) {
  WordStats ws(3);
  std::string filename = "test.word_stats";
  EXPECT_TRUE(ws.LoadAndMerge(std::vector<std::string>(1, filename)));

  int32_t word_count = 0;
  for (WordStats::ConstIterator it(&ws); !it.Done(); it.Next()) {
    EXPECT_EQ(&(it.GetTopicHistogram()), &(ws.GetTopicHistogram(it.Word())));
    ++word_count;
  }
  EXPECT_EQ(word_count, ws.NumWords());
}

TEST(WordStatsTest, AppendAsString) {
  WordStats ws(3);
  std::string filename = "test.word_stats";
  EXPECT_TRUE(ws.LoadAndMerge(std::vector<std::string>(1, filename)));

  std::string s;
  ws.AppendAsString(&s, NULL);
  EXPECT_EQ("100\t2:5 1:3 \n"
            "101\t2:5 1:3 \n"
            "102\t2:5 1:3 \n"
            "103\t2:5 1:3 \n"
            "104\t2:5 1:3 \n", s);

  Vocabulary v;
  for (int i = 0; i < 102; ++i) {
    std::string tmp;
    StringAppendF(&tmp, "w%03d", i);
    v.AddWord(tmp);
  }

  s.clear();
  ws.AppendAsString(&s, &v);
  EXPECT_EQ("w100\t2:5 1:3 \n"
            "w101\t2:5 1:3 \n"
            "UNKNOWN_102\t2:5 1:3 \n"
            "UNKNOWN_103\t2:5 1:3 \n"
            "UNKNOWN_104\t2:5 1:3 \n", s);
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
