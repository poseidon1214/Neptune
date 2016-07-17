// Copyright (c) 2014 Tencent Inc.
// Author: Li Meng (elvisli@tencent.com)

#ifndef COMMON_CONFIG_BASE_CONFIG_MANAGER_H_
#define COMMON_CONFIG_BASE_CONFIG_MANAGER_H_

#include <string>
#include <tr1/type_traits>

#include "common/base/singleton.h"
#include "common/base/static_assert.h"

namespace google {
namespace protobuf {
class Message;
}  // namespace protobuf
}  // namespace google

namespace gdt {

template <typename T>
class BaseConfigManager : public gdt::SingletonBase<BaseConfigManager<T> > {
 public:
  BaseConfigManager() {
    STATIC_ASSERT(
      (std::tr1::is_base_of<google::protobuf::Message, T>::value),
      "template type IS NOT derived from google::protobuf::Message");
  }
  ~BaseConfigManager() {}

  bool Load(const std::string& file_path);
  const T& Get() const;

 private:
  T config_;
};

template <typename T>
bool BaseConfigManager<T>::Load(const std::string& file_path) {
  bool BaseConfigManager_LoadAndParse(const std::string & file_path,
                                      google::protobuf::Message * message);
  return BaseConfigManager_LoadAndParse(file_path, &config_);
}

template <typename T>
const T& BaseConfigManager<T>::Get() const {
  return config_;
}

}  // namespace gdt

#endif  // COMMON_CONFIG_BASE_CONFIG_MANAGER_H_

