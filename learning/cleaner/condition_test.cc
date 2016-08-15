// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
// Author: Wang Qian <cernwang@tencent.com>

#include <string>
#include <map>
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"
#include "learning/cleaner/condition.h"
#include "learning/proto/test.pb.h"

using std::string;
using namespace gdt::learning;

bool JudgeCommodity(const Commodity& commodity) {
  return commodity.commodity_id() > commodity.source_id();
}

bool BiggerThan(const Commodity& commodity, uint64_t value) {
  return commodity.commodity_id() + commodity.source_id() > value;
}


TEST(Condition, AndConditionTest) {
  shared_ptr<Condition<Commodity> > equal_condition;
  equal_condition.reset(new RawCondition<Commodity>());
  static_cast<RawCondition<Commodity>* >(equal_condition.get())->Set([](const Commodity& commodity)->bool {
                                                                       return commodity.commodity_id() == 10;
                                                                     });
  shared_ptr<Condition<Commodity> > function_condition;
  function_condition.reset(new RawCondition<Commodity>());
  static_cast<RawCondition<Commodity>* >(function_condition.get())->Set(&JudgeCommodity);

  AndCondition<Commodity> condition;
  condition.Add(equal_condition);
  condition.Add(function_condition);
  Commodity commodity1;
  commodity1.set_commodity_id(3);
  commodity1.set_source_id(2);
  EXPECT_EQ(condition.Satisfy(commodity1), false);
  Commodity commodity2;
  commodity2.set_commodity_id(10);
  commodity2.set_source_id(12);
  EXPECT_EQ(condition.Satisfy(commodity2), false);
  Commodity commodity3;
  commodity3.set_commodity_id(3);
  commodity3.set_source_id(9);
  EXPECT_EQ(condition.Satisfy(commodity3), false);
  Commodity commodity4;
  commodity4.set_commodity_id(10);
  commodity4.set_source_id(8);
  EXPECT_EQ(condition.Satisfy(commodity4), true);
}



TEST(Condition, OrConditionTest) {
  shared_ptr<Condition<Commodity> > equal_condition;
  equal_condition.reset(new RawCondition<Commodity>());
  static_cast<RawCondition<Commodity>* >(equal_condition.get())->Set([](const Commodity& commodity)->bool {
                                                                       return commodity.commodity_id() == 10;
                                                                     });
  shared_ptr<Condition<Commodity> > function_condition;
  function_condition.reset(new RawCondition<Commodity>());
  static_cast<RawCondition<Commodity>* >(function_condition.get())->Set(&JudgeCommodity);
  OrCondition<Commodity> condition;
  condition.Add(equal_condition);
  condition.Add(function_condition);

  Commodity commodity1;
  commodity1.set_commodity_id(3);
  commodity1.set_source_id(2);
  EXPECT_EQ(condition.Satisfy(commodity1), true);
  Commodity commodity2;
  commodity2.set_commodity_id(10);
  commodity2.set_source_id(12);
  EXPECT_EQ(condition.Satisfy(commodity2), true);
  Commodity commodity3;
  commodity3.set_commodity_id(3);
  commodity3.set_source_id(9);
  EXPECT_EQ(condition.Satisfy(commodity3), false);
  Commodity commodity4;
  commodity4.set_commodity_id(10);
  commodity4.set_source_id(8);
  EXPECT_EQ(condition.Satisfy(commodity4), true);
}

TEST(Condition, NotConditionTest) {
  shared_ptr<Condition<Commodity> > function_condition;
  function_condition.reset(new RawCondition<Commodity>());
  static_cast<RawCondition<Commodity>* >(function_condition.get())->Set(&JudgeCommodity);
  NotCondition<Commodity> condition;
  condition.Set(function_condition);
  Commodity commodity1;
  commodity1.set_commodity_id(3);
  commodity1.set_source_id(2);
  EXPECT_EQ(condition.Satisfy(commodity1), false);
  Commodity commodity2;
  commodity2.set_commodity_id(10);
  commodity2.set_source_id(12);
  EXPECT_EQ(condition.Satisfy(commodity2), true);
}
