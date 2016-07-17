// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)
//         Yi Wang (yiwang@tencent.com

#include <unistd.h>                     // for access
#include <sys/stat.h>                   // for lstat

#include <string>
#include <tr1/unordered_map>

#include "thirdparty/gtest/gtest.h"

#include "app/qzap/text_analysis/topic/base/model.h"

namespace qzap {
namespace text_analysis {
namespace base {

using std::string;
using std::tr1::unordered_map;

const int32_t kNumTopics = 6;
const int32_t kVocabSize = 5;

const std::string kModelDir = "testdata/model-standard";
const std::string kVocabFile = "testdata/vocab";

bool FileExistsAndNotEmpty(const char* filename) {
  if (access(filename, F_OK) != -1) {
    struct stat st;
    lstat(filename, &st);
    if (!S_ISDIR(st.st_mode) && st.st_size > 0) {
      return true;
    }
  }
  return false;
}

TEST(ModelTest, Save) {
  const char* kOutputModelDir = "lda_model";

  Model m(kNumTopics);

  DenseTopicHistogram wth, gth;
  wth.Resize(kNumTopics);
  wth[0] = 3;
  wth[1] = 5;
  wth[2] = 3;
  wth[3] = 5;
  wth[4] = 3;
  gth.Resize(kNumTopics);

  unordered_map<int32_t, DenseTopicHistogram> ws_map;
  for (int32_t w = 100; w < 100 + kVocabSize; ++w) {
    ws_map[w] = wth;
    for (int32_t t = 0; t < kNumTopics; ++t) {
      gth[t] += wth[t];
    }
  }

  m.word_stats_->ParseFrom(ws_map);
  m.global_stats_->ParseFrom(gth);
  m.hyperparams_->Set(0.1, kNumTopics, 0.01, kVocabSize);

  EXPECT_EQ(m.NumTopics(), kNumTopics);
  EXPECT_EQ(m.VocabSize(), kVocabSize);

  m.Save(kOutputModelDir);

  string hyperparams_filename = string(kOutputModelDir) + "/lda.hyperparams";
  EXPECT_TRUE(FileExistsAndNotEmpty(hyperparams_filename.c_str()));

  string word_stats_filename = string(kOutputModelDir) + "/lda.word_stats";
  EXPECT_TRUE(FileExistsAndNotEmpty(word_stats_filename.c_str()));

  string global_stats_filename = string(kOutputModelDir) + "/lda.global_stats";
  EXPECT_TRUE(FileExistsAndNotEmpty(global_stats_filename.c_str()));
}


TEST(ModelTest, Load) {
  Model model(kModelDir.c_str());

  EXPECT_EQ(2, model.NumTopics());
  EXPECT_EQ(6, model.VocabSize());

  const Hyperparams& hyperparams = *(model.hyperparams_);
  EXPECT_DOUBLE_EQ(0.1, hyperparams.TopicPrior(0));
  EXPECT_DOUBLE_EQ(0.1, hyperparams.TopicPrior(1));
  EXPECT_DOUBLE_EQ(0.01, hyperparams.WordPrior());
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

int main(int ac, char** av) {
  ::testing::InitGoogleTest(&ac, av);
  if (!google::ParseCommandLineFlags(&ac, &av, true)) {
    LOG(ERROR) << "parse command line failed";
    return -1;
  }
  return RUN_ALL_TESTS();
}
