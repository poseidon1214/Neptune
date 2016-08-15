// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
// Author: Wang Qian <cernwang@tencent.com>

#include <string>
#include <map>

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"
#include "learning/evaluate/evaluater.h"
#include "learning/proto/test.pb.h"

using std::string;
using namespace gdt::learning;

bool PctrCompare(const Exposure& left, const Exposure& right) {
  return left.pctr() > right.pctr();
}

bool PctrNegativeCompare(const Exposure& left, const Exposure& right) {
  return left.pctr() < right.pctr();
}

bool GroundTruth(const Exposure& exposure) {
  return exposure.click() > 0;
}

TEST(Evaluater, AucComputeTest) {
  std::vector<Exposure> exposures;
  {
    Exposure exposure;
    exposure.set_pctr(0.1);
    exposure.set_click(0);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.2);
    exposure.set_click(0);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.3);
    exposure.set_click(0);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.4);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.5);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.6);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.9);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  Evaluater evaluater;
  EXPECT_EQ(evaluater.AucCompute<Exposure>(&exposures, &PctrCompare, &GroundTruth), 1);
  EXPECT_EQ(evaluater.AucCompute<Exposure>(&exposures, &PctrNegativeCompare, &GroundTruth), 0);
  exposures.clear();
  {
    Exposure exposure;
    exposure.set_pctr(0.1);
    exposure.set_click(0);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.2);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.3);
    exposure.set_click(0);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.4);
    exposure.set_click(0);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.5);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.6);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.9);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  EXPECT_DOUBLE_EQ(evaluater.AucCompute<Exposure>(&exposures, &PctrCompare, &GroundTruth), 1 - static_cast<double>(2) / 12);
}

TEST(Evaluater, SpearmanRankTest) {
  std::vector<Exposure> exposures;
  {
    Exposure exposure;
    exposure.set_hour(0);
    exposure.set_click(0);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(1);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(2);
    exposure.set_click(2);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(3);
    exposure.set_click(3);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(4);
    exposure.set_click(4);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(5);
    exposure.set_click(5);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(6);
    exposure.set_click(6);
    exposures.push_back(exposure);
  }
  Evaluater evaluater;
  EXPECT_EQ(evaluater.SpearmanRank(&exposures, &Exposure::click, &Exposure::hour), 1);
  exposures.clear();
  {
    Exposure exposure;
    exposure.set_hour(170);
    exposure.set_click(180);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(150);
    exposure.set_click(165);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(210);
    exposure.set_click(190);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(180);
    exposure.set_click(168);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(160);
    exposure.set_click(172);
    exposures.push_back(exposure);
  }
  EXPECT_DOUBLE_EQ(evaluater.SpearmanRank(&exposures, &Exposure::click, &Exposure::hour), 0.7);
}


TEST(Evaluater, PearsonCoefficientTest) {
  std::vector<Exposure> exposures;
  {
    Exposure exposure;
    exposure.set_hour(0);
    exposure.set_click(0);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(1);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(2);
    exposure.set_click(2);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(3);
    exposure.set_click(3);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(4);
    exposure.set_click(4);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(5);
    exposure.set_click(5);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_hour(6);
    exposure.set_click(6);
    exposures.push_back(exposure);
  }
  Evaluater evaluater;
  EXPECT_DOUBLE_EQ(evaluater.PearsonCoefficient(&exposures, &Exposure::click, &Exposure::hour), 1);
  exposures.clear();
  {
    Exposure exposure;
    exposure.set_pctr(0.11);
    exposure.set_click(1);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.12);
    exposure.set_click(2);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.13);
    exposure.set_click(3);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.15);
    exposure.set_click(5);
    exposures.push_back(exposure);
  }
  {
    Exposure exposure;
    exposure.set_pctr(0.18);
    exposure.set_click(8);
    exposures.push_back(exposure);
  }
  EXPECT_LE(1 - evaluater.PearsonCoefficient(&exposures, &Exposure::click, &Exposure::pctr), 0.00001);
}


TEST(Evaluater, MIC) {
  std::vector<Exposure> exposures;
  size_t n = 1000;
  double PI = 3.14159265;
  exposures.resize(n);
  for (size_t i = 0; i < n; i++) {
    /* build x = [0, 0.001, ..., 1] */
    exposures[i].set_pctr((double) i / (double) (n-1));
    /* build y = sin(10 * pi * x) + x */
    exposures[i].set_fix_pctr(sin(10 * PI * exposures[i].pctr()) + exposures[i].pctr());
  }
  EvaluateResult result;
  Evaluater evaluater;
  EXPECT_TRUE(evaluater.MutualInformationCoff(&exposures, &Exposure::pctr, &Exposure::fix_pctr, &result));
  LOG(ERROR) << result.Utf8DebugString();
  LOG(ERROR) << evaluater.PearsonCoefficient(&exposures, &Exposure::fix_pctr, &Exposure::pctr);
}