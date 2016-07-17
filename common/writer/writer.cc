// Copyright (c) 2014, Tencent Inc.
// Author: Wang Qian<cernwang@tencent.com>

#include "common/writer/writer.h"

#include <fstream>
#include "app/qzap/common/utility/time_utility.h"

namespace gdt {

using namespace std;  // NOLINT(build/namespaces)
using namespace google::protobuf;  // NOLINT(build/namespaces)


std::ostream& Writer::operator << (std::ostream& os, 
                                   const google::protobuf::Message& message) {
  const Reflection* reflection = message.GetReflection();
  vector<const FieldDescriptor*> fields;
  reflection->ListFields(message, &fields);

  for (size_t i = 0; i < fields.size(); i++) {
    const FieldDescriptor* field = fields[i];
    switch (field->cpp_type()) {
#define CASE_FIELD_TYPE(cpptype, method) \
      case FieldDescriptor::CPPTYPE_##cpptype: { \
        if (field->is_repeated()) { \
          int field_size = reflection->FieldSize(message, field); \
          for (int index = 0; index < field_size; index++) { \
            os << reflection->GetRepeated##method( \
                  message, field, index) << ";"; \
          } \
        }  else { \
          os << reflection->Get##method(message, field); \
        } \
        break; \
      }

      CASE_FIELD_TYPE(INT32,  Int32);
      CASE_FIELD_TYPE(UINT32, UInt32);
      CASE_FIELD_TYPE(DOUBLE, Double);
      CASE_FIELD_TYPE(INT64,  Int64);
      CASE_FIELD_TYPE(UINT64, UInt64);
      CASE_FIELD_TYPE(BOOL, Bool);
      CASE_FIELD_TYPE(FLOAT,  Float);
#undef CASE_FIELD_TYPE

      case FieldDescriptor::CPPTYPE_STRING: {
        string field_value;
        if (field->is_repeated()) {
          int field_size = reflection->FieldSize(message, field);
          for (int index = 0; index < field_size; index++) {
            const string& value = reflection->GetRepeatedStringReference(
                message, field, index, &field_value);
            os << value << ";";
          }
        } else {
          const string& value = reflection->GetStringReference(
              message, field, &field_value);
          os << value;
        }
        break;
      }
      case FieldDescriptor::CPPTYPE_ENUM: {
        if (field->is_repeated()) {
          int field_size = reflection->FieldSize(message, field);
          for (int index = 0; index < field_size; index++) {
            os << reflection->GetRepeatedEnum(message, field, index)->number() << ";";
          }
        } else {
          os << reflection->GetEnum(message, field)->number();
        }
        break;
      }
      case FieldDescriptor::CPPTYPE_MESSAGE: {
        if (field->is_repeated()) {
          break;
        } else {
          os << reflection->GetMessage(message, field);
        }
        break;
      }
    }
    if (field->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE) {
      os << "\t";
    }
  }
}

}  // namespace gdt
