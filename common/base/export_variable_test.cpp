// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/base/export_variable.h"
#include <stdint.h>
#include <string>
#include "common/system/concurrency/atomic/atomic.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/jsoncpp/writer.h"

namespace gdt {

// Export global variable
int g_test_count;
EXPORT_VARIABLE(test_count, &g_test_count);

// Export global function
int CallCount() {
  return g_test_count;
}
EXPORT_VARIABLE(call_count, CallCount);

static std::string DumpGroup(ExportedVariableGroup* group) {
  Json::Value value;
  group->Dump(&value);
  Json::FastWriter writer;
  return writer.write(value);
}

static std::string DumpRoot() {
  return DumpGroup(ExportedVariable::Root());
}

TEST(ExportedVariables, GlobalTest) {
  EXPECT_EQ("{\"call_count\":0,\"test_count\":0}\n", DumpRoot());
  ++g_test_count;
  EXPECT_EQ("{\"call_count\":1,\"test_count\":1}\n", DumpRoot());
}

class TestClass {
 public:
  TestClass() :
    m_field(1),
    m_const_method_reg("xxx", this, &TestClass::Xxx),
    m_non_const_method_reg("yyy", this, &TestClass::Yyy),
    m_reg_field("field", &m_field) {
  }
  std::string Xxx() const {
    return "xxx";
  }
  // non-const function
  std::string Yyy() {
    return "yyy";
  }
 private:
  int m_field;
  VariableRegisterer m_const_method_reg;
  VariableRegisterer m_non_const_method_reg;
  VariableRegisterer m_reg_field;
};

TEST(ExportedVariables, ScopeTest) {
  g_test_count = 0;
  EXPECT_EQ("{\"call_count\":0,\"test_count\":0}\n", DumpRoot());
  {
    TestClass t;
    EXPECT_EQ("{\"call_count\":0,\"field\":1,\"test_count\":0,\"xxx\":\"xxx\","
              "\"yyy\":\"yyy\"}\n", DumpRoot());
  }
  EXPECT_EQ("{\"call_count\":0,\"test_count\":0}\n", DumpRoot());
}

class TestClass2 {
 public:
  explicit TestClass2(
      ExportedVariableGroup* registry = ExportedVariable::Root()) :
    m_field(1),
    m_variables(registry) {
    m_variables.Export("xxx", this, &TestClass2::Xxx);
    m_variables.Export("yyy", this, &TestClass2::Yyy);
    m_variables.Export("field", &m_field);
  }
  std::string Xxx() const {
    return "xxx";
  }
  // non-const function
  std::string Yyy() {
    return "yyy";
  }
 private:
  int m_field;
  VariableExporter m_variables;
};

TEST(ExportedVariables, VariableExporter) {
  g_test_count = 0;
  EXPECT_EQ("{\"call_count\":0,\"test_count\":0}\n", DumpRoot());
  {
    TestClass2 t;
    EXPECT_EQ("{\"call_count\":0,\"field\":1,\"test_count\":0,\"xxx\":\"xxx\","
              "\"yyy\":\"yyy\"}\n", DumpRoot());
  }
  EXPECT_EQ("{\"call_count\":0,\"test_count\":0}\n", DumpRoot());
}

ExportedVariableGroup g_test_registry;
EXPORT_VARIABLE_TO_GROUP(&g_test_registry, test_count_in_registry,
                         &g_test_count);

TEST(ExportedVariables, GlobalExportedVariableGroup) {
  EXPECT_EQ("{\"test_count_in_registry\":0}\n", DumpGroup(&g_test_registry));
}

ExportedVariableGroup g_type_test_registry;

static short s = 0;  // NOLINT(runtime/int)
static short us = 0;  // NOLINT(runtime/int)
static long l = 0;  // NOLINT(runtime/int)
static unsigned long ul = 0;  // NOLINT(runtime/int)
static long long ll = 0;  // NOLINT(runtime/int)
static unsigned long ull = 0;  // NOLINT(runtime/int)
static float f = 0;
static double d = 0;

EXPORT_VARIABLE_TO_GROUP(&g_type_test_registry, s, &s);
EXPORT_VARIABLE_TO_GROUP(&g_type_test_registry, us, &us);
EXPORT_VARIABLE_TO_GROUP(&g_type_test_registry, l, &l);
EXPORT_VARIABLE_TO_GROUP(&g_type_test_registry, ul, &ul);
EXPORT_VARIABLE_TO_GROUP(&g_type_test_registry, ll, &ll);
EXPORT_VARIABLE_TO_GROUP(&g_type_test_registry, ull, &ull);
EXPORT_VARIABLE_TO_GROUP(&g_type_test_registry, f, &f);
EXPORT_VARIABLE_TO_GROUP(&g_type_test_registry, d, &d);

TEST(ExportedVariables, Types) {
  EXPECT_EQ("{\"d\":0.0,\"f\":0.0,\"l\":0,\"ll\":0,\"s\":0,\"ul\":0,"
            "\"ull\":0,\"us\":0}\n",
            DumpGroup(&g_type_test_registry));
}

ExportedVariableGroup g_atomic_test_registry;

static Atomic<int64_t> i64 = 0;

EXPORT_VARIABLE_TO_GROUP(&g_atomic_test_registry, i64, &i64);

TEST(ExportedVariables, Atomic) {
  EXPECT_EQ("{\"i64\":0}\n", DumpGroup(&g_atomic_test_registry));
}

TEST(ExportedVariables, VariableExporterWithRegistry) {
  ExportedVariableGroup registry;
  g_test_count = 0;
  EXPECT_EQ("{}\n", DumpGroup(&registry));
  {
    TestClass2 t(&registry);
    EXPECT_EQ("{\"field\":1,\"xxx\":\"xxx\",\"yyy\":\"yyy\"}\n",
              DumpGroup(&registry));
  }
  EXPECT_EQ("{}\n", DumpGroup(&registry));
}

TEST(ExportedVariables, SameNamedRegisterer) {
  TestClass t1;
  TestClass t2;
}

TEST(ExportedVariables, SameNamedExporter) {
  TestClass2 t1;
  TestClass2 t2;
}

}  // namespace gdt
