// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
// Author: Wang Qian <cernwang@tencent.com>

#include <string>
#include <map>
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"
#include "learning/cleaner/lambda_parser.h"
#include "learning/proto/test.pb.h"

using std::string;
using namespace gdt::learning;

TEST(LambdaParser, LambdaParserTest) {
  LambdaParser<Commodity> lambda_parser;
  gdt::learning::Lambda lambda;
  lambda.set_logical_operator(gdt::learning::LOGICAL_OPERATOR_OR);
  gdt::learning::Expression* expression = lambda.add_expression();
  expression->set_math_operator(gdt::learning::MATH_OPERATOR_EQUAL);
  expression->mutable_left_field()->add_field_name("commodity_id");
  expression->mutable_right_field()->mutable_field_value()->set_data_type(FieldValue::UINT64);
  expression->mutable_right_field()->mutable_field_value()->set_uint64_value(10);
  expression = lambda.add_expression();
  expression->set_math_operator(gdt::learning::MATH_OPERATOR_BIGGER_THAN);
  expression->mutable_left_field()->add_field_name("commodity_id");
  expression->mutable_right_field()->add_field_name("source_id");
  LOG(ERROR) << lambda.Utf8DebugString();
  shared_ptr<Condition<Commodity> > condition;
  EXPECT_TRUE(lambda_parser.Parse(lambda, condition));

  Commodity commodity1;
  commodity1.set_commodity_id(3);
  commodity1.set_source_id(2);
  EXPECT_EQ(condition->Satisfy(commodity1), true);

  Commodity commodity2;
  commodity2.set_commodity_id(10);
  commodity2.set_source_id(12);
  EXPECT_EQ(condition->Satisfy(commodity2), true);
  Commodity commodity3;
  commodity3.set_commodity_id(3);
  commodity3.set_source_id(9);
  EXPECT_EQ(condition->Satisfy(commodity3), false);
  Commodity commodity4;
  commodity4.set_commodity_id(10);
  commodity4.set_source_id(8);
  EXPECT_EQ(condition->Satisfy(commodity4), true);
}


TEST(Condition, AndConditionTest) {
  LambdaParser<Commodity> lambda_parser;
  gdt::learning::Lambda lambda;
  lambda.set_logical_operator(gdt::learning::LOGICAL_OPERATOR_AND);
  gdt::learning::Expression* expression = lambda.add_expression();
  expression->set_math_operator(gdt::learning::MATH_OPERATOR_EQUAL);
  expression->mutable_left_field()->add_field_name("commodity_id");
  expression->mutable_right_field()->mutable_field_value()->set_data_type(FieldValue::UINT64);
  expression->mutable_right_field()->mutable_field_value()->set_uint64_value(10);
  expression = lambda.add_expression();
  expression->set_math_operator(gdt::learning::MATH_OPERATOR_BIGGER_THAN);
  expression->mutable_left_field()->add_field_name("commodity_id");
  expression->mutable_right_field()->add_field_name("source_id");
  LOG(ERROR) << lambda.Utf8DebugString();
  shared_ptr<Condition<Commodity> > condition;
  EXPECT_TRUE(lambda_parser.Parse(lambda, condition));

  Commodity commodity1;
  commodity1.set_commodity_id(3);
  commodity1.set_source_id(2);
  EXPECT_EQ(condition->Satisfy(commodity1), false);
  Commodity commodity2;
  commodity2.set_commodity_id(10);
  commodity2.set_source_id(12);
  EXPECT_EQ(condition->Satisfy(commodity2), false);
  Commodity commodity3;
  commodity3.set_commodity_id(3);
  commodity3.set_source_id(9);
  EXPECT_EQ(condition->Satisfy(commodity3), false);
  Commodity commodity4;
  commodity4.set_commodity_id(10);
  commodity4.set_source_id(8);
  EXPECT_EQ(condition->Satisfy(commodity4), true);
}