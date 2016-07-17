// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 12/14/11
// Description:

#include "common/base/export_variable.h"

#include <map>
#include <string>

#include "common/base/singleton.h"
#include "thirdparty/jsoncpp/value.h"

namespace gdt {

ExportedVariableGroup* ExportedVariable::Root() {
  return &Singleton<ExportedVariableGroup>::Instance();
}

/////////////////////////////////////////////////////////////////////////////
// ExportedVariableGroup staff

ExportedVariableGroup::ExportedVariableGroup() {}

ExportedVariableGroup::~ExportedVariableGroup() {}

bool ExportedVariableGroup::First(std::string* name, ExportedVariable** var) {
  MutexLocker locker(&m_mutex);
  if (!m_variables.empty()) {
    MapType::iterator iter = m_variables.begin();
    *name = iter->first;
    *var = iter->second;
    return true;
  }
  return false;
}

ExportedVariable* ExportedVariableGroup::FindByName(const std::string& name) {
  MutexLocker locker(&m_mutex);
  MapType::const_iterator iter = m_variables.find(name);
  if (iter != m_variables.end()) {
    return iter->second;
  }
  return NULL;
}

bool ExportedVariableGroup::Register(const std::string& name,
                                     ExportedVariable* var) {
  MutexLocker locker(&m_mutex);
  return m_variables.insert(std::make_pair(name, var)).second;
}

bool ExportedVariableGroup::Unregister(const std::string& name) {
  MutexLocker locker(&m_mutex);
  return m_variables.erase(name) == 1U;
}

void ExportedVariableGroup::Dump(Json::Value* value) const {
  *value = Json::objectValue;
  MutexLocker locker(&m_mutex);
  for (MapType::const_iterator i = m_variables.begin();
       i != m_variables.end(); ++i) {
    Json::Value var_value;
    i->second->Dump(&var_value);
    (*value)[i->first] = var_value;
  }
}

/////////////////////////////////////////////////////////////////////////////
// VariableRegisterer staff

VariableRegisterer::~VariableRegisterer() {
  if (m_variable) {
    if (m_registry)
      m_registry->Unregister(m_name);
    delete m_variable;
  }
}

void VariableRegisterer::Register(ExportedVariable* var) {
  if (!m_registry || !m_registry->Register(m_name, var)) {
    delete var;
    m_variable = NULL;
    return;
  }

  m_variable = var;
}

/////////////////////////////////////////////////////////////////////////////
// VariableExporter staff

VariableExporter::VariableExporter() : m_registry(ExportedVariable::Root()) {}

VariableExporter::VariableExporter(ExportedVariableGroup* registry) :
  m_registry(registry) {}

VariableExporter::~VariableExporter() {
  std::map<std::string, ExportedVariable*>::iterator i;
  for (i = m_variables.begin(); i != m_variables.end(); ++i) {
    if (m_registry)
      m_registry->Unregister(i->first);
    delete i->second;
  }
  m_variables.clear();
}

bool VariableExporter::Register(const std::string& name,
                                ExportedVariable* variable) {
  if (!m_registry->Register(name, variable)) {
    delete variable;
    return false;
  }
  m_variables[name] = variable;
  return true;
}

}  // namespace gdt
