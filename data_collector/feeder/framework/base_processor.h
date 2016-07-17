// Copyright (c) 2015, Tencent Inc.
// Author: cernwang<cernwang@tencent.com>
// Processor基类

#ifndef FRAMEWORK_BASE_PROCESSOR_H_
#define FRAMEWORK_BASE_PROCESSOR_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "data_collector/feeder/framework/base_functor.h"
#include "data_collector/feeder/framework/data_message.h"

namespace gdt {

template <class ConfigType = FeederConfig, class DataMessageType = DataMessage>
class BaseProcessor {
 public:
  BaseProcessor() : success_init_(false) {
  }

  virtual ~BaseProcessor() {
  }

  virtual bool Init() {
    success_init_ = true;
    return true;
  }

  virtual bool SyncConf(ConfigType* config) {
    config_ = config;
    return true;
  }

  virtual void UnInit() {
    for (size_t i = 0; i < base_functor_list_.size(); ++i) {
      if (base_functor_list_[i]) {
        base_functor_list_[i]->UnInit();
      }
      delete base_functor_list_[i];
    }
    base_functor_list_.clear();
  }

  virtual bool AddFunctor(BaseFunctor<ConfigType, DataMessageType>* base_functor) {
    if (!base_functor || !base_functor->SyncConf(config_) || !base_functor->Init()) {
      LOG(ERROR) << "Add Functor Failed NULL or Not Init";
      return false;
    }
    base_functor_list_.push_back(base_functor);
    return true;
  }

  virtual bool DoProcess(DataMessageType* data_message) {
    if (!success_init_) {
      LOG(ERROR) << "Not success init\t" << this->ProcessorName();
      return false;
    }
    if (!BeginWork()) {
      LOG(ERROR) << "Begin work Failed\t" << this->ProcessorName();
      return false;
    }
    if (!DoWork(data_message)) {
      LOG(ERROR) << "Do work Failed\t" << this->ProcessorName();
      return false;
    }
    EndWork();
    return true;
  }

  virtual std::string ProcessorName() {
    return "BaseProcessor";
  }

  virtual bool DoWork(DataMessageType* data_message) {
                      LOG(ERROR) << "base_functor_list_\t" <<base_functor_list_.size();

    for (int j = 0 ; j < base_functor_list_.size(); j++) {
      LOG(ERROR) << base_functor_list_[j]->Name() << " do work begin";
      if (base_functor_list_[j]->DoWork(data_message) != kSuccess) {
        LOG(ERROR) << base_functor_list_[j]->Name() << " do work failed";
      }
    }
    return true;
  }

  virtual bool BeginWork() {
    return true;
  }

  virtual void EndWork() {
  }

 public:
  bool success_init_;
  // 配置
  const ConfigType* config_;
  // functor列表
  std::vector<BaseFunctor<ConfigType, DataMessageType>* > base_functor_list_;
};

typedef BaseProcessor<FeederConfig, DataMessage> FeederBaseProcessor;

#define INIT_FUNCTOR(functor_name, functor_type) \
  auto* functor_name(new functor_type()); \
  CHECK_NOTNULL(functor_name); \
  if (!AddFunctor(functor_name)) { \
    LOG(ERROR) << "AddFunctor " << #functor_type << " failed" ; \
    return false; \
  } else {  \
    LOG(ERROR) << "AddFunctor " << #functor_type << " suceess" ; \ 
  }



}  // namespace gdt
#endif  // FRAMEWORK_BASE_PROCESSOR_H_

