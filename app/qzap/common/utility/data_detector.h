// Copyright (c) 2015, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>
#ifndef APP_QZAP_COMMON_UTILITY_DATA_DETECTOR_H_
#define APP_QZAP_COMMON_UTILITY_DATA_DETECTOR_H_
#include <cstdio>

#include <string>
#include <vector>

#include "common/base/shared_ptr.h"
#include "common/base/scoped_mmap.h"

namespace gdt {
class DataDetector {
 public:
  explicit DataDetector(const std::string &pattern);

  virtual ~DataDetector() {
  }

  virtual bool TryDetectingNewestData();

  shared_ptr<ScopedMMap> GetCurrentMemory() {
    return memory_;
  }

  std::string GetCurrentName() {
    return name_;
  }

 private:
  static bool GetNewestFile(const std::string &pattern, std::string *name);

 private:
  std::string pattern_;
  shared_ptr<ScopedMMap> memory_;
  std::string name_;
};

}  // namespace gdt

#endif  // APP_QZAP_COMMON_UTILITY_DATA_DETECTOR_H_
