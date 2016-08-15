// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#ifndef RETRIEVAL_BASE_RETRIEVALER_H_
#define RETRIEVAL_BASE_RETRIEVALER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <tuple>
#include "retrieval/proto/document.pb.h"
#include "retrieval/proto/query.pb.h"
#include "common/base/class_register.h"

namespace gdt {

// 类余弦检索
class Retrievaler {
 public:
  bool Init() {
    return true;
  }
  bool Build(const std::vector<Document>& documents);
  bool Retrieval(const Query& query, std::vector<Result>* results);
};

CLASS_REGISTER_DEFINE_REGISTRY(Retrievaler_register, Retrievaler);

#define REGISTER_RETRIEVALER(functor_name) \
  CLASS_REGISTER_OBJECT_CREATOR( \
    Retrievaler_register, Retrievaler, \
      #functor_name, functor_name)

#define CREATE_RETRIEVALER(functor_name_as_string) \
  CLASS_REGISTER_CREATE_OBJECT(Retrievaler_register, \
    functor_name_as_string)

}  // namespace gdt

#endif  // RETRIEVAL_BASE_RETRIEVALER_H_
