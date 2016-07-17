// Copyright (c) 2011 Tencent Inc.
// Author: Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/classifier/time.h"
#include <cmath>
#include "thirdparty/gtest/gtest.h"

namespace qzap {
namespace text_analysis {

TEST(TimeTest, MoreTics) {
  Time t;
  t.Tic();
}

TEST(TimeTest, MoreTocs) {
  Time t;
  EXPECT_DOUBLE_EQ(t.Toc(), -1.0);
}

TEST(TimeTest, TicToc) {
  Time t;

  t.Tic();
  {
    t.Tic();
    {
      double sum = 0;
      for (size_t i = 0; i < 1000000; ++i) {
        sum += sqrt((2.0 * i) * i);
      }
    }
    LOG(INFO) << "time 1: " << t.Toc() << " ms";

    t.Tic();
    {
      double sum = 0;
      for (size_t i = 0; i < 1000000; ++i) {
        sum += sqrt((2.0 * i) * i);
      }
    }
    LOG(INFO) << "time 2: " << t.Toc() << " ms";
  }
  LOG(INFO) << "time 3: " << t.Toc() << " ms";
}

TEST(TimeTest, StackDepth) {
  Time t;
  EXPECT_EQ(0u, t.StackDepth());

  t.Tic();
  t.Tic();
  EXPECT_EQ(2u, t.StackDepth());

  t.Toc();
  t.Toc();
}

TEST(TimeTest, Clear) {
  Time t;
  t.Tic();
  t.Tic();
  t.Clear();
  EXPECT_EQ(0u, t.StackDepth());
}

TEST(TimeTest, GlobalTicToc) {
  Tic();
  {
    Tic();
    {
      double sum = 0;
      for (size_t i = 0; i < 1000000; ++i) {
        sum += sqrt((2.0 * i) * i);
      }
    }
    LOG(INFO) << "time 1: " << Toc() << " ms";

    Tic();
    {
      double sum = 0;
      for (size_t i = 0; i < 1000000; ++i) {
        sum += sqrt((2.0 * i) * i);
      }
    }
    LOG(INFO) << "time 2: " << Toc() << " ms";
  }
  LOG(INFO) << "time 3: " << Toc() << " ms";
}

}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
