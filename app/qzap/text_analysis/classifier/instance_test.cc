// Copyright (c) 2011 Tencent Inc.
// Author: ZhiQiang Chen (lucienchen@tencent.com)

#include "app/qzap/text_analysis/classifier/instance.h"

#include <map>

#include "thirdparty/gtest/gtest.h"

const double kEps = 1E-6;

namespace qzap {
namespace text_analysis {

TEST(Instance, L1Norm) {
  Instance instance;
  instance.AddFeature(1, 0.32);
  instance.AddFeature(2, 0.56);
  EXPECT_NEAR(0.88, instance.L1Norm(), 0.001);
}

TEST(Instance, L2Norm) {
  Instance instance;
  instance.AddFeature(1, 0.5);
  instance.AddFeature(2, 0.6);
  EXPECT_NEAR(0.78, instance.L2Norm(), 0.005);
}

TEST(Instance, L1NormalizeNoZero) {
  Instance instance;
  instance.AddFeature(1, 0.32);
  instance.AddFeature(2, 0.56);
  instance.L1Normalize();
  EXPECT_EQ(1u, instance.IdAt(0));
  EXPECT_EQ(2u, instance.IdAt(1));
  EXPECT_NEAR(0.364, instance.WeightAt(0), 0.001);
  EXPECT_NEAR(0.636, instance.WeightAt(1), 0.001);
}

TEST(Instance, L1NormalizeZero) {
  Instance instance;
  instance.AddFeature(1, 0.0);
  instance.AddFeature(2, 0.0);
  instance.L1Normalize();
  EXPECT_EQ(1u, instance.IdAt(0));
  EXPECT_EQ(2u, instance.IdAt(1));
  EXPECT_NEAR(0.0, instance.WeightAt(0), kEps);
  EXPECT_NEAR(0.0, instance.WeightAt(1), kEps);
}

TEST(Instance, L2NormalizeNoZero) {
  Instance instance;
  instance.AddFeature(1, 0.5);
  instance.AddFeature(2, 0.6);
  instance.L2Normalize();
  EXPECT_EQ(1u, instance.IdAt(0));
  EXPECT_EQ(2u, instance.IdAt(1));
  EXPECT_NEAR(0.641, instance.WeightAt(0), 0.01);
  EXPECT_NEAR(0.769, instance.WeightAt(1), 0.01);
}

TEST(Instance, L2NormalizeZero) {
  Instance instance;
  instance.AddFeature(1, 0.0);
  instance.AddFeature(2, 0.0);
  instance.L2Normalize();
  EXPECT_EQ(1u, instance.IdAt(0));
  EXPECT_EQ(2u, instance.IdAt(1));
  EXPECT_NEAR(0.0, instance.WeightAt(0), kEps);
  EXPECT_NEAR(0.0, instance.WeightAt(1), kEps);
}

TEST(Instance, AddNoExist) {
  Instance instance;
  instance.AddFeature(1, 0.5);
  EXPECT_NEAR(0.5, instance.WeightAt(0), 0.001);
}

TEST(Instance, ParseFrom) {
  // ParseFromNonEmptyString
  Instance instance;
  std::string text = "1:0.5 2:0.7 3:0.8";
  instance.ParseFrom(text);
  EXPECT_NEAR(0.5, instance.WeightAt(0), kEps);
  EXPECT_NEAR(0.7, instance.WeightAt(1), kEps);
  EXPECT_NEAR(0.8, instance.WeightAt(2), kEps);
  instance.Clear();
  // ParseFromInvalidString
  text = "";
  instance.ParseFrom(text);
  EXPECT_TRUE(instance.Empty());
}

TEST(Instance, SerializeTo) {
  // SerializeNonEmptyInstanceToString
  Instance instance;
  instance.AddFeature(1, 0.5);
  instance.AddFeature(2, 0.8);
  std::string text = "";
  instance.SerializeTo(&text);
  EXPECT_EQ("1:0.500000 2:0.800000", text);
  instance.Clear();
  // SerializeEmptyInstanceToString
  text = "";
  instance.SerializeTo(&text);
  EXPECT_EQ("", text);
}

TEST(Instance, SortByIdTest) {
  Instance instance;
  instance.AddFeature(2, 0.5);
  instance.AddFeature(1, 0.8);
  std::string text = "";
  instance.SortById();
  instance.SerializeTo(&text);
  EXPECT_EQ("1:0.800000 2:0.500000", text);
}

TEST(Instance, SortByWeightTest) {
  Instance instance;
  instance.AddFeature(1, 0.5);
  instance.AddFeature(2, 0.8);
  std::string text = "";
  instance.SortByWeight();
  instance.SerializeTo(&text);
  EXPECT_EQ("2:0.800000 1:0.500000", text);
}

} // namespace text_analysis
} // namespace qzap

