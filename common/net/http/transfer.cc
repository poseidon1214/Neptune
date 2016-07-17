// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/net/http/transfer.h"
#include "thirdparty/glog/logging.h"

namespace gdt {

static int DoDetectHttpHeaderSize(StringPiece buffer) {
  static const char kHeaderEnd_1[] = "\r\n\r\n";
  static const size_t kHeaderEndLength_1 = 4;
  // Here we assume there is no malformed http message.
  const char* p = reinterpret_cast<const char*>(
      memmem(buffer.data(), buffer.size(), kHeaderEnd_1, kHeaderEndLength_1));
  if (p != NULL) {
    return p - buffer.data() + kHeaderEndLength_1;
  }
  static const char kHeaderEnd_2[] = "\n\n";
  static const size_t kHeaderEndLength_2 = 2;
  p = reinterpret_cast<const char*>(
      memmem(buffer.data(), buffer.size(), kHeaderEnd_2, kHeaderEndLength_2));
  if (p != NULL) {
    return p - buffer.data() + kHeaderEndLength_2;
  }
  return 0;
}

int DetectHttpHeaderSize(StringPiece buffer) {
  int result = DoDetectHttpHeaderSize(buffer);
  if (result > static_cast<int>(kMaxHttpHeadersSize)) {
    LOG(WARNING) << "Http Header size too large: " << result;
    return -1;
  }
  if (result == 0 && buffer.size() > kMaxHttpHeadersSize) {
    LOG(WARNING) << "Http Header size too large, can't reach end until "
                 << buffer.size() << " bytes";
    return -1;
  }
  return result;
}

}  // namespace gdt
