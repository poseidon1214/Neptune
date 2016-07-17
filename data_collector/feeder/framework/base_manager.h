// Copyright (c) 2015, Tencent Inc.
// Author: cernwang<cernwang@tencent.com>
// Processor基类

#ifndef FRAMEWORK_BASE_MANAGER_H_
#define FRAMEWORK_BASE_MANAGER_H_

#include <iostream>
#include <fstream>
#include "data_collector/feeder/framework/base_processor.h"
#include "data_collector/feeder/framework/data_message.h"

namespace gdt {


template <class ConfigType = FeederConfig, class DataMessageType = DataMessage>
class BaseManager {
 public:
  typedef BaseConfigManager<ConfigType> ConfigChecker;

 public:
  BaseManager() : success_init_(false) {
  }

  virtual bool LoadConf(const std::string& conf) {
    PASS_OR_RETURN(ConfigChecker::Instance().Load(conf));
    config_ = &ConfigChecker::Instance().Get();
    return true;
  }

  virtual ~BaseManager() {}

  virtual bool AddProcessor(BaseProcessor<ConfigType, DataMessageType>* base_processor) {
    if (!base_processor || !base_processor->SyncConf(config_) || !base_processor->Init()) {
      LOG(ERROR) << "Add Functor Failed NULL or Not Init";
      return false;
    }
    base_processor_list_.push_back(base_processor);
    return true;
  }

  virtual  bool DoProcess() {
    shared_ptr<DataMessageType> data_message(new DataMessageType());
    for (size_t i = 0; i < base_processor_list_.size(); ++i) {
      if (base_processor_list_[i]) {
        LOG(INFO) << "Do Process " << base_processor_list_[i]->ProcessorName();
        base_processor_list_[i]->DoProcess(data_message.get());
      }
    }
    return true;
  }

  virtual void UnInit() {
    for (size_t i = 0; i < base_processor_list_.size(); ++i) {
      if (base_processor_list_[i]) {
        base_processor_list_[i]->UnInit();
      }
      delete base_processor_list_[i];
    }
    base_processor_list_.clear();
  }

  virtual const std::string Name() {
    return "BaseManager";
  }

 public:
  bool success_init_;
  // 配置
  const ConfigType* config_;
  // processor列表
  std::vector<BaseProcessor<ConfigType, DataMessageType>* > base_processor_list_;
};

typedef BaseManager<FeederConfig, DataMessage> FeederBaseManager;

#define INIT_PROCESSOR(processor_name, processor_type) \
  auto* processor_name(new processor_type()); \
  CHECK_NOTNULL(processor_name); \
  if (!AddProcessor(processor_name)) { \
    LOG(ERROR) << "AddProcessor " << #processor_type << " failed" ; \
    return false; \
  }


}
#endif  // FRAMEWORK_BASE_MANAGER_H_