// Copyright (c) 2011 Tencent Inc.
// Author: Zhao Xuemin (xueminzhao@tencent.com)
//         Pan Yang (baileyyang@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#ifndef APP_QZAP_TEXT_ANALYSIS_DICT_DICT_IO_H_
#define APP_QZAP_TEXT_ANALYSIS_DICT_DICT_IO_H_

#include <stddef.h>    // for ptrdiff_t
#include <iostream>
#include <string>
#include <tr1/memory>  // for std::tr1::shared_ptr<>
#include <vector>

#include "thirdparty/glog/logging.h"

namespace qzap {
namespace text_analysis {

// write string to ostream
// note: not considering io fault
inline int WriteString(const std::string& str, std::ostream& out) {
  std::string::size_type len = str.size();
  out.write(reinterpret_cast<char*>(&len), sizeof(len));
  out.write(str.c_str(), len);
  return static_cast<int>(len + sizeof(len));
}

// read string from istream
// note: not considering io fault
inline int ReadString(std::istream& in, std::string* str) {
  std::string::size_type len;
  in.read(reinterpret_cast<char*>(&len), sizeof(len));
  std::vector<char> tmp(len);
  in.read(&tmp[0], len);
  str->assign(&tmp[0], len);
  return static_cast<int>(len + sizeof(len));
}

template<typename MessageType>
int WriteProtobufMessage(const MessageType& msg, std::ostream& out) {
  std::string str;
  msg.SerializeToString(&str);
  return WriteString(str, out);
}

template<typename MessageType>
int ReadProtobufMessage(std::istream& in, MessageType* msg) {
  std::string str;
  int offset = ReadString(in, &str);
  msg->ParseFromString(str);
  return offset;
}

//  POD = Plain Old Type
template<typename PODType>
int WritePOD(const PODType& pod, std::ostream& out) {
  out.write(reinterpret_cast<const char*>(&pod), sizeof(pod));
  return sizeof(pod);
}

template<typename PODType>
int ReadPOD(std::istream &in, PODType *pod) {
  in.read(reinterpret_cast<char*>(pod), sizeof(*pod));
  return sizeof(*pod);
}

// container's value_type is Message
template<typename ValueType, typename ForwardIterator>
int WriteProtobufMessagesAux(ForwardIterator first,
                             ForwardIterator last,
                             std::ostream& out,
                             ValueType*) {
  int offset = 0;
  while (first != last) {
    offset += WriteProtobufMessage(*first, out);
    ++first;
  }
  return offset;
}

// container's value_type is Message*
template<typename ValueType, typename ForwardIterator>
int WriteProtobufMessagesAux(ForwardIterator first,
                             ForwardIterator last,
                             std::ostream& out,
                             ValueType**) {
  int offset = 0;
  while (first != last) {
    offset += WriteProtobufMessage(**first, out);
    ++first;
  }
  return offset;
}

// container's value_type is shared_ptr<Message>
template<typename ValueType, typename ForwardIterator>
int WriteProtobufMessagesAux(ForwardIterator first,
                             ForwardIterator last,
                             std::ostream& out,
                             std::tr1::shared_ptr<ValueType>*) {
  int offset = 0;
  while (first != last) {
    offset += WriteProtobufMessage(**first, out);
    ++first;
  }
  return offset;
}

// write protobuf messages to ostream
// note: container's valuetype is Message or Message* or shared_ptr<Message>
template<typename ForwardIterator>
int WriteProtobufMessages(ForwardIterator first,
                          ForwardIterator last,
                          std::ostream& out) {
  ptrdiff_t n = std::distance(first, last);
  if (n < 0) {
    LOG(ERROR) << "last-first < 0";
    return -1;
  }

  out.write(reinterpret_cast<char*>(&n), sizeof(n));
  return sizeof(n) + WriteProtobufMessagesAux(first, last, out,
      reinterpret_cast<typename ForwardIterator::value_type*>(NULL));
}

// container's value_type is Message
template<typename ValueType, typename ContainerType>
int ReadProtobufMessagesAux(std::istream& in,
                            ptrdiff_t n,
                            ContainerType* c,
                            ValueType*) {
  int offset = 0;
  ValueType msg;
  while(n > 0) {
    offset += ReadProtobufMessage(in, &msg);
    c->push_back(msg);
    --n;
  }
  return offset;
}

// container's value_type is Message*
template<typename ValueType, typename ContainerType>
int ReadProtobufMessagesAux(std::istream& in,
                            ptrdiff_t n,
                            ContainerType* c,
                            ValueType**) {
  int offset = 0;
  ValueType msg;
  while(n > 0) {
    offset += ReadProtobufMessage(in, &msg);
    c->push_back(new ValueType(msg));
    --n;
  }
  return offset;
}

// container's value_type is shared_ptr<Message>
template<typename ValueType, typename ContainerType>
int ReadProtobufMessagesAux(std::istream& in,
                            ptrdiff_t n,
                            ContainerType* c,
                            std::tr1::shared_ptr<ValueType>*) {
  int offset = 0;
  ValueType msg;
  while(n > 0) {
    offset += ReadProtobufMessage(in, &msg);
    c->push_back(std::tr1::shared_ptr<ValueType>(new ValueType(msg)));
    --n;
  }
  return offset;
}

// read protobuf messages from istream to container
// note: container's valuetype is Message or Message* or shared_ptr<Message>
// note: protobuf messages will be push_back into container
template<typename ContainerType>
int ReadProtobufMessages(std::istream& in, ContainerType* c) {
  ptrdiff_t n;
  in.read(reinterpret_cast<char*>(&n), sizeof(n));
  if (n < 0) {
    LOG(ERROR) << "data length < 0";
    return -1;
  }
  return sizeof(n) + ReadProtobufMessagesAux(in, n, c,
      reinterpret_cast<typename ContainerType::value_type*>(NULL));
}

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_DICT_DICT_IO_H_

