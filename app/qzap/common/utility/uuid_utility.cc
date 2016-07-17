// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-4-8
#include <string>
#include "thirdparty/uuid/uuid.h"
std::string GenerateUUID() {
  static const int kMaxUUIDLength = 37;
  char buf[kMaxUUIDLength + 1] = { 0 };
  uuid_t id;
  uuid_clear(id);
  uuid_generate(id);
  uuid_unparse_lower(id, buf);
  return std::string(buf);
}
