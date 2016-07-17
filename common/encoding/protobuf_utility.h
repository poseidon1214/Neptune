// Copyright (C), 1998-2015, Tencent
// Author: Li, Jiliang neoli@tencent.com
// Date: 2015年11月25日

#ifndef APP_QZAP_COMMON_UTILITY_PROTOBUF_UTILITY_H_
#define APP_QZAP_COMMON_UTILITY_PROTOBUF_UTILITY_H_

#include <string>

namespace google {
namespace protobuf {
class Message;
}
}

namespace gdt {
bool SerializeToGzippedString(const google::protobuf::Message& message,
    std::string *output);
bool ParseFromGzippedString(const std::string& input,
    google::protobuf::Message* message);
google::protobuf::Message* CreateProtoMessageByName(const std::string& name);
}  // namespace gdt

#endif  // APP_QZAP_COMMON_UTILITY_PROTOBUF_UTILITY_H_
