// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

// 对商品数据库的操作的类
#ifndef LEARNING_CLEANER_CONDITIONS_H_
#define  LEARNING_CLEANER_CONDITIONS_H_


#include <cstdlib>
#include <iostream>
#include <string>
#include <map>

#include "app/qzap/common/base/shared_ptr.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"

namespace gdt {
namespace learning {

template <class T>
class Condition {
 public:
  //
  virtual bool Satisfy(const T& t) {
    return true;
  };
  // 析构
  virtual ~Condition() {};
};

//
template <class T>
class RawCondition : public Condition<T> {
 public:
  //
  std::function<bool (const T&)> JudgeValue_;
  //
  bool Satisfy(const T& t) {
    return JudgeValue_(t);
  }
  //
  void Set(std::function<bool (const T&)> JudgeValue) {
    JudgeValue_ = JudgeValue;
  }
};

template <class T>
class AndCondition : public Condition<T> {
 public:
  //
  std::vector<shared_ptr<Condition<T> > > conditions;
  //
  bool Satisfy(const T& t) {
    for (auto iter = conditions.begin();
        iter != conditions.end(); iter++) {
      if (!(**iter).Satisfy(t)) {
        return false;
      }
    }
    return true;
  }
  //
  void Add(shared_ptr<Condition<T> > condition) {
    conditions.push_back(condition);
  }
};

template <class T>
class OrCondition : public Condition<T> {
 public:
  //
  std::vector<shared_ptr<Condition<T> > > conditions;
  //
  bool Satisfy(const T& t) {
    for (auto iter = conditions.begin();
        iter != conditions.end(); iter++) {
      if ((**iter).Satisfy(t)) {
        return true;
      }
    }
    return false;
  }
  //
  void Add(shared_ptr<Condition<T> > condition) {
    conditions.push_back(condition);
  }
};

template <class T>
class NotCondition : public Condition<T> {
 public:
  //
  shared_ptr<Condition<T> > condition;
  //
  bool Satisfy(const T& t) {
    return !condition->Satisfy(t);
  }
  //
  void Set(shared_ptr<Condition<T> > c) {
    condition = c;
  }
};

}  // namespace learning
}  // namespace gdt

#endif  // LEARNING_CLEANER_CONDITIONS_H_
