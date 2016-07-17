// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: Huan Yu <huanyu@tencent.com>

#ifndef COMMON_ENCODING_COMPARE_PROTO_H_
#define COMMON_ENCODING_COMPARE_PROTO_H_

#include <functional>

namespace google {
namespace protobuf {

class Message;

}  // namespace protobuf
}  // namespace google

namespace gdt {

// Compare the serialized strings, make message comparable.
// Return >0, 0, or < 0.
int ProtoCompare(const google::protobuf::Message& lhs,
                 const google::protobuf::Message& rhs);

// Return true if lhs exactly equals to rhs.
bool ProtoEqual(const google::protobuf::Message& lhs,
                const google::protobuf::Message& rhs);

// Return true if we can get same values from coresponding fields from lhs and
// rhs.
//
// Field with default value is "Equivalent" to unset field, but not "Equal".
//
// It uses protobuf reflection to compare two messages recursively.
// Unknown fields are ignored.
bool ProtoEquivalent(const google::protobuf::Message& lhs,
                     const google::protobuf::Message& rhs);

// Functor for STL
struct ProtoLess : std::binary_function <google::protobuf::Message,
                                         google::protobuf::Message, bool> {
  bool operator()(const google::protobuf::Message& lhs,
                  const google::protobuf::Message& rhs) {
    return ProtoCompare(lhs, rhs) < 0;
  }
};

// Functor for STL
struct ProtoGreater : std::binary_function<google::protobuf::Message,
                                           google::protobuf::Message, bool> {
  bool operator()(const google::protobuf::Message& lhs,
                  const google::protobuf::Message& rhs) {
    return ProtoCompare(lhs, rhs) > 0;
  }
};

}  // namespace gdt

#endif  // COMMON_ENCODING_COMPARE_PROTO_H_
