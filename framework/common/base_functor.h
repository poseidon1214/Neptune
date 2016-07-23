// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef FRAMEWORK_BASE_FUNCTOR_H_
#define FRAMEWORK_BASE_FUNCTOR_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"


#define PASS_OR_RETURN(result)  \
  if (!(result)) { \
    LOG(ERROR) << #result << " == false"; \
    return false; \
  }

namespace gdt {

enum FunctorResult {
  kUnKnown = 0,
  kSuccess = 1,
  kFailedContinued = 2,
  kFailedNotContinued = 3,
};

template <class ConfigType, class DataMessageType>
class BaseFunctor {
 public:
  BaseFunctor() : success_init_(false) {
  }

  virtual ~BaseFunctor() {}

  virtual bool Init() {
    success_init_ = true;
    return true;
  }

  virtual bool SyncConf(ConfigType* config) {
    config_ = config;
    return true;
  }

  virtual void UnInit() {
  }

  virtual const std::string Name() {
    return "BaseFunctor";
  }

  virtual FunctorResult DoWork(DataMessageType* data_message) {
    return kSuccess;
  }

 public:
  bool success_init_;
  // 配置
  const ConfigType* config_;
};

#define REGISTER_FUNCTOR(functor_name, functor_type) \
  CLASS_REGISTER_OBJECT_CREATOR( \
    functor_type##_register, functor_type, \
      #functor_name, functor_name)

#define CREATE_FUNCTOR(functor_name_as_string, functor_type) \
  CLASS_REGISTER_CREATE_OBJECT(functor_type##_register, \
    functor_name_as_string)

}  // namespace gdt
#endif  // FRAMEWORK_BASE_FUNCTOR_H_