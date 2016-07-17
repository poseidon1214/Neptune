// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: Huan Yu <huanyu@tencent.com>

#include "common/encoding/compare_proto.h"
#include <math.h>
#include <vector>
#include "thirdparty/protobuf/descriptor.h"
#include "thirdparty/protobuf/message.h"

namespace gdt {

using google::protobuf::FieldDescriptor;
using google::protobuf::Message;
using google::protobuf::Reflection;

int ProtoCompare(const Message& lhs, const Message& rhs) {
  return lhs.SerializeAsString().compare(rhs.SerializeAsString());
}

bool ProtoEqual(const Message& lhs, const Message& rhs) {
  return (lhs.SerializeAsString() == rhs.SerializeAsString());
}

// specialize template function for float, double compare
template<typename T>
bool IsEqual(const T& left, const T& right) {
  return left == right;
}

template<>
bool IsEqual(const float& left, const float& right) {
  return (fabsf(left - right) < 1e-6);
}

template<>
bool IsEqual(const double& left, const double& right) {
  return (fabs(left - right) < 1e-15);
}

#define CASE_FIELD_TYPE(cpptype, method)                                 \
  case FieldDescriptor::CPPTYPE_##cpptype: {                             \
    if (field->is_repeated()) {                                          \
      int field_size = reflection->FieldSize(lhs, field);                \
      int rhs_field_size = reflection->FieldSize(rhs, field);            \
      if (field_size != rhs_field_size) {                                \
        return false;                                                    \
      }                                                                  \
      for (int index = 0; index < field_size; ++index) {                 \
        if (!IsEqual(reflection->GetRepeated##method(lhs, field, index), \
            reflection->GetRepeated##method(rhs, field, index))) {       \
          return false;                                                  \
        }                                                                \
      }                                                                  \
    } else {                                                             \
      if (!IsEqual(reflection->Get##method(lhs, field),                  \
          reflection->Get##method(rhs, field))) {                        \
        return false;                                                    \
      }                                                                  \
    }                                                                    \
    break;                                                               \
  }

static bool InternalCompare(const Message& lhs, const Message& rhs) {
  const Reflection* reflection = lhs.GetReflection();
  std::vector<const FieldDescriptor*> fields;
  reflection->ListFields(lhs, &fields);

  for (size_t i = 0; i < fields.size(); ++i) {
    const FieldDescriptor* field = fields[i];
    switch (field->cpp_type()) {
      CASE_FIELD_TYPE(INT32, Int32);
      CASE_FIELD_TYPE(UINT32, UInt32);
      CASE_FIELD_TYPE(INT64, Int64);
      CASE_FIELD_TYPE(UINT64, UInt64);
      CASE_FIELD_TYPE(FLOAT, Float);
      CASE_FIELD_TYPE(DOUBLE, Double);
      CASE_FIELD_TYPE(BOOL, Bool);
      CASE_FIELD_TYPE(STRING, String);
      CASE_FIELD_TYPE(ENUM, Enum);

    case FieldDescriptor::CPPTYPE_MESSAGE: {
      if (field->is_repeated()) {
        int field_size = reflection->FieldSize(lhs, field);
        int rhs_field_size = reflection->FieldSize(rhs, field);
        if (field_size != rhs_field_size) {
          return false;
        }
        for (int index = 0; index < field_size; index++) {
          if (!InternalCompare(
                reflection->GetRepeatedMessage(lhs, field, index),
                reflection->GetRepeatedMessage(rhs, field, index))) {
            return false;
          }
        }
      } else {
        if (!InternalCompare(reflection->GetMessage(lhs, field),
                             reflection->GetMessage(rhs, field))) {
          return false;
        }
      }
      break;
    }
    }
  }
  return true;
}

bool ProtoEquivalent(const Message& lhs, const Message& rhs) {
  if (lhs.GetDescriptor() != rhs.GetDescriptor()) {
    return false;
  }
  return InternalCompare(lhs, rhs) && InternalCompare(rhs, lhs);
}

}  // namespace gdt
