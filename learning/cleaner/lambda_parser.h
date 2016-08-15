// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

// 对商品数据库的操作的类
#ifndef LEARNING_CLEANER_LAMBDA_PARSER_H_
#define LEARNING_CLEANER_LAMBDA_PARSER_H_


#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include "common/proto/config.pb.h"
#include "common/encoding/proto_converter.h"
#include "learning/cleaner/condition.h"
#include "learning/proto/condition.pb.h"
#include "framework/common/base_functor.h"
#include "learning/proto/test.pb.h"

namespace gdt {
namespace learning {

static bool operator == (const FieldValue& left_field, const FieldValue& right_field) {
  switch (left_field.data_type()) {
    case FieldValue::INT32:
      return left_field.int32_value() == right_field.int32_value();
    case FieldValue::INT64:
      return left_field.int32_value() == right_field.int64_value();
    case FieldValue::UINT32:
      return left_field.uint32_value() == right_field.uint32_value();
    case FieldValue::UINT64:
      return left_field.uint64_value() == right_field.uint64_value();        
    case FieldValue::DOUBLE:
      return left_field.double_value() == right_field.double_value();
    case FieldValue::FLOAT:
      return left_field.float_value() == right_field.float_value();  
    case FieldValue::BOOL:
      return left_field.bool_value() == right_field.bool_value();
    case FieldValue::STRING:
      return left_field.string_value() == right_field.string_value();
  }
}
static bool operator < (const FieldValue& left_field, const FieldValue& right_field) {
  switch (left_field.data_type()) {
    case FieldValue::INT32:
      return left_field.int32_value() < right_field.int32_value();
    case FieldValue::INT64:
      return left_field.int32_value() < right_field.int64_value();
    case FieldValue::UINT32:
      return left_field.uint32_value() < right_field.uint32_value();
    case FieldValue::UINT64:
      return left_field.uint64_value() < right_field.uint64_value();        
    case FieldValue::DOUBLE:
      return left_field.double_value() < right_field.double_value();
    case FieldValue::FLOAT:
      return left_field.float_value() < right_field.float_value();  
    case FieldValue::BOOL:
      return left_field.bool_value() < right_field.bool_value();
    case FieldValue::STRING:
      return left_field.string_value() < right_field.string_value();
  }
}

template <class T>
static bool SatisfyExpression(const T& t, const Expression& expression) {
  FieldValue left_field, right_field;
  if (expression.left_field().has_field_value()) {
    left_field.CopyFrom(expression.left_field().field_value());
  } else {
    CHECK_LOG(GetFieldValue(&t, expression.left_field().field_name(), &left_field),
              expression.left_field().Utf8DebugString());
  }
  if (expression.right_field().has_field_value()) {
    right_field.CopyFrom(expression.right_field().field_value());
  } else {
    CHECK_LOG(GetFieldValue(&t, expression.right_field().field_name(), &right_field),
              expression.right_field().Utf8DebugString());
  }
  switch (expression.math_operator()) {
    case MATH_OPERATOR_EQUAL:
      return left_field == right_field;
    case MATH_OPERATOR_LESS_THAN:
      return left_field < right_field;
    case MATH_OPERATOR_BIGGER_THAN :
      return right_field < left_field;
  }
}

template <class T>
class LambdaParser {
 public:
  bool Parse(const Expression& expression, shared_ptr<Condition<T> >& condition) {
    condition.reset(new RawCondition<T>());
    auto satisfy = [&] (const T& t)->bool {
                     return SatisfyExpression(t, expression);
                   };
    static_cast<RawCondition<T>* >(condition.get())->Set(satisfy);
    return true;
  }

  bool Parse(const Lambda& lambda, shared_ptr<Condition<T> >& condition) {
    shared_ptr<Condition<T> > origin_condition;
    if (lambda.expression_size() == 1 && 
        lambda.lambda().empty()) {
      CHECK_LOG(Parse(lambda.expression(0), origin_condition),
                      lambda.expression(0).Utf8DebugString());
    } else {
      switch (lambda.logical_operator()) {
        case LOGICAL_OPERATOR_AND:
          origin_condition.reset(new AndCondition<T>());
          for (auto iter = lambda.lambda().begin();
               iter != lambda.lambda().end(); iter++) {
            shared_ptr<Condition<T> > child_conditon;
            CHECK_LOG(Parse(*iter, child_conditon),
                      iter->Utf8DebugString());
            static_cast<AndCondition<T>* >(origin_condition.get())->Add(child_conditon);
          }
          for (auto iter = lambda.expression().begin();
               iter != lambda.expression().end(); iter++) {
            shared_ptr<Condition<T> > child_conditon;
            CHECK_LOG(Parse(*iter, child_conditon),
                      iter->Utf8DebugString());
            static_cast<AndCondition<T>* >(origin_condition.get())->Add(child_conditon);
          }
          break;
        case LOGICAL_OPERATOR_OR:
          origin_condition.reset(new OrCondition<T>());
          for (auto iter = lambda.lambda().begin();
               iter != lambda.lambda().end(); iter++) {
            shared_ptr<Condition<T> > child_conditon;
            CHECK_LOG(Parse(*iter, child_conditon),
                      iter->Utf8DebugString());
            static_cast<OrCondition<T>* >(origin_condition.get())->Add(child_conditon);
          }
          for (auto iter = lambda.expression().begin();
               iter != lambda.expression().end(); iter++) {
            shared_ptr<Condition<T> > child_conditon;
            CHECK_LOG(Parse(*iter, child_conditon),
                      iter->Utf8DebugString());
            static_cast<OrCondition<T>* >(origin_condition.get())->Add(child_conditon);
          }
          break;
      }

    }
    if (lambda.not_operator()) {
      condition.reset(new NotCondition<T>());
      static_cast<NotCondition<T>* >(condition.get())->Set(origin_condition);
    } else {
      condition = origin_condition;
    }
    return true;
  }
};

}  // namespace learning
}  // namespace gdt

#endif  // LEARNING_CLEANER_LAMBDA_PARSER_H_
