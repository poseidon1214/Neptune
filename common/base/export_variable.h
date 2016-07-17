// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_BASE_EXPORT_VARIABLE_H_
#define COMMON_BASE_EXPORT_VARIABLE_H_

#include <stddef.h>
#include <map>
#include <string>
#include <vector>
#include "common/base/static_assert.h"
#include "common/base/type_traits.h"
#include "common/base/uncopyable.h"
#include "common/system/concurrency/mutex.h"
#include "thirdparty/jsoncpp/json.h"

template <typename T> class Atomic;

namespace gdt {

class ExportedVariableGroup;

class ExportedVariable {
  DECLARE_UNCOPYABLE(ExportedVariable);
 protected:
  ExportedVariable() {}
 public:
  virtual ~ExportedVariable() {}
  virtual void Dump(Json::Value* value) const = 0;
  static ExportedVariableGroup* Root();
};

// Registry represent a group of exported variables
class ExportedVariableGroup : public ExportedVariable {
  DECLARE_UNCOPYABLE(ExportedVariableGroup);
  friend class VariableRegisterer;
  friend class VariableExporter;

 public:
  ExportedVariableGroup();
  ~ExportedVariableGroup();
  bool First(std::string* name, ExportedVariable** var);
  ExportedVariable* FindByName(const std::string& name);
  virtual void Dump(Json::Value* value) const;

 private:
  bool Register(const std::string& name, ExportedVariable* var);
  bool Unregister(const std::string& name);

 private:
  mutable Mutex m_mutex;
  typedef std::map<std::string, ExportedVariable*> MapType;
  MapType m_variables;
};

namespace internal {

// This helper function is used to bypass JsonCpp bug:
// Convert long and unsigned long to Json::Value is ambiguous.
// but (u)int64_t in <stdint.h> is actual long under LP64 platform.

template <typename T>
void ToJsonValue(const T& value, Json::Value* json_value) {
  *json_value = value;
}

inline void ToJsonValue(long value, Json::Value* json_value) { // NOLINT
  *json_value = static_cast<Json::Int64>(value);
}

inline void ToJsonValue(unsigned long value, Json::Value* json_value) { // NOLINT
  *json_value = static_cast<Json::UInt64>(value);
}

template <typename T>
void ToJsonValue(const Atomic<T>& value, Json::Value* json_value) {
  ToJsonValue(value.Value(), json_value);
}

template <typename T>
class ExportedNormalVariable : public ExportedVariable {
 public:
  explicit ExportedNormalVariable(const T* ptr) : m_ptr(ptr) {}

  virtual void Dump(Json::Value* value) const {  // override
    ToJsonValue(*m_ptr, value);
  }

 private:
  const T* m_ptr;
};

template <typename T>
class ExportedFunctionVariable : public ExportedVariable {
 public:
  explicit ExportedFunctionVariable(T(*function)()) : m_function(function) {}

  virtual void Dump(Json::Value* value) const {  // override
    ToJsonValue(m_function(), value);
  }

 private:
  T(*m_function)();
};

template <typename T, typename Class>
class ExportedConstMethodVariable : public ExportedVariable {
  typedef T(Class::*MethodType)() const;

 public:
  explicit ExportedConstMethodVariable(
    const Class* object,
    T(Class::*method)() const)
    :
    m_object(object), m_method(method) {
  }


  virtual void Dump(Json::Value* value) const {  // override
    ToJsonValue((m_object->*m_method)(), value);
  }

 private:
  const Class* m_object;
  MethodType m_method;
};

template <typename T, typename Class>
class ExportedNonConstMethodVariable : public ExportedVariable {
  typedef T(Class::*MethodType)();

 public:
  explicit ExportedNonConstMethodVariable(
    Class* object,
    T(Class::*method)())
    :
    m_object(object), m_method(method) {
  }

  virtual void Dump(Json::Value* value) const {  // override
    ToJsonValue((m_object->*m_method)(), value);
  }

 private:
  Class* m_object;
  MethodType m_method;
};

}  // namespace internal

// Register one variable when constructed, and unregister it automatically
// when destructed.
class VariableRegisterer {
 public:
  template <typename T>
  VariableRegisterer(const std::string& name,
                     T* address,
                     ExportedVariableGroup* registry = ExportedVariable::Root())
    : m_registry(registry), m_name(name) {
    Register(new internal::ExportedNormalVariable<T>(address));
  }

  template <typename T>
  VariableRegisterer(const std::string& name,
                     T(*function)(),
                     ExportedVariableGroup* registry = ExportedVariable::Root())
    : m_registry(registry), m_name(name) {
    Register(new internal::ExportedFunctionVariable<T>(function));
  }

  template <typename T, typename Class>
  VariableRegisterer(const std::string& name,
                     const Class* object,
                     T(Class::*method)() const,
                     ExportedVariableGroup* registry = ExportedVariable::Root())
    : m_registry(registry), m_name(name) {
    Register(
        new internal::ExportedConstMethodVariable<T, Class>(object, method));
  }

  template <typename T, typename Class>
  VariableRegisterer(const std::string& name,
                     Class* object,
                     T(Class::*method)(),
                     ExportedVariableGroup* registry = ExportedVariable::Root())
    : m_registry(registry), m_name(name) {
    Register(
        new internal::ExportedNonConstMethodVariable<T, Class>(object, method));
  }

  ~VariableRegisterer();

 private:
  VariableRegisterer(const VariableRegisterer&);
  VariableRegisterer& operator=(const VariableRegisterer&);
  ExportedVariableGroup* Registry();
  void Register(ExportedVariable* var);

 private:
  ExportedVariableGroup* m_registry;
  const std::string m_name;
  ExportedVariable* m_variable;
};

// Export multiple variables as a group, and unregister all variables register
// by it automatically.
//
// It is useful when you want to export multiple variables but don't want to
// unregister them one by one.
//
class VariableExporter {
 public:
  // Export to default registry.
  VariableExporter();

  // Export to user specified registry.
  explicit VariableExporter(ExportedVariableGroup* registry);

  ~VariableExporter();

  template <typename T>
  bool Export(const std::string& name, T* address) {
    return Register(name, new internal::ExportedNormalVariable<T>(address));
  }

  template <typename T>
  bool Export(const std::string& name, T(*function)()) {
    return Register(name, new internal::ExportedFunctionVariable<T>(function));
  }

  template <typename T, typename Class>
  bool Export(
    const std::string& name,
    const Class* object,
    T(Class::*method)() const) {
    return Register(
        name,
        new internal::ExportedConstMethodVariable<T, Class>(object, method));
  }

  template <typename T, typename Class>
  bool Export(
    const std::string& name,
    Class* object,
    T(Class::*method)()) {
    return Register(
        name,
        new internal::ExportedNonConstMethodVariable<T, Class>(object, method));
  }

 private:
  bool Register(const std::string& name, ExportedVariable* variable);
  ExportedVariableGroup* Registry();

 private:
  ExportedVariableGroup* m_registry;
  std::map<std::string, ExportedVariable*> m_variables;
};

}  // namespace gdt

// Register a variable to specified group.
#define EXPORT_VARIABLE_TO_GROUP(registry, name, ...) \
namespace registered_variables { \
    VariableRegisterer g_##variable_##name##_register(#name, __VA_ARGS__, \
                                                      registry); \
}

// Register a variable to global registry.
#define EXPORT_VARIABLE(name, ...) \
    EXPORT_VARIABLE_TO_GROUP(ExportedVariable::Root(), name, __VA_ARGS__)

#endif  // COMMON_BASE_EXPORT_VARIABLE_H_
