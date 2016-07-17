// Copyright (c) 2015, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>
#include "app/qzap/common/utility/data_detector.h"

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <algorithm>

#include "app/qzap/common/utility/file_utility.h"

namespace gdt {
DataDetector::DataDetector(const std::string &pattern)
  : pattern_(pattern) {
}

bool DataDetector::GetNewestFile(
    const std::string &pattern,
    std::string *name) {
  std::vector<std::string> filenames;
  if (!Glob(pattern, 0, &filenames)) {
    VLOG(1) << "Glob pattern = " << pattern;
    return false;
  }
  if (filenames.empty()) {
    VLOG(1) << "filesnames empty()";
    return false;
  }
  std::sort(filenames.begin(), filenames.end());
  *name = *filenames.rbegin();
  return true;
}

bool DataDetector::TryDetectingNewestData() {
  std::string name;
  if (!GetNewestFile(pattern_, &name)) {
    VLOG(1) << "GetNewestFile pattern_ = " << pattern_;
    return false;
  }
  if (name == name_) {
    return false;
  }
  int32_t fd = open(name.c_str(), O_RDWR, S_IWUSR | S_IRUSR);
  if (fd == -1) {
    PLOG(ERROR) << "open fail " << name;
    return false;
  }
  AutoCloseFd l(fd);
  struct stat st;
  if (fstat(fd, &st) != 0) {
    PLOG(ERROR) << "Get fstat fail " << name;
    return false;
  }
  size_t file_size = st.st_size;
  if (file_size == 0) {
    VLOG(1) << "file size == 0";
    return false;
  }
  void* ptr = mmap(0, file_size, PROT_READ, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    PLOG(ERROR) << "Mapp failed";
    return false;
  }
  shared_ptr<ScopedMMap> memory(new ScopedMMap(ptr, file_size));
  memory_ = memory;
  name_ = name;
  return true;
}

}  // namespace gdt
