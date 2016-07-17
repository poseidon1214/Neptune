// Copyright (c) 2015, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>

#ifndef COMMON_BASE_SCOPED_MMAP_H_
#define COMMON_BASE_SCOPED_MMAP_H_
#include <sys/mman.h>
#include "thirdparty/glog/logging.h"

// scoped mmap, is just like scoped pointer
// auto munmap a ptr

namespace gdt {

class ScopedMMap {
 public:
  ScopedMMap(void *ptr, size_t size)
  : ptr_(ptr), size_(size) {
  }

  ~ScopedMMap() {
    if (ptr_ != NULL) {
      if (munmap(ptr_, size_) == 0) {
        ptr_ = NULL;
        size_ = 0u;
      } else {
        PLOG(ERROR) << "mumap fail";
      }
    }
  }

  const char* ptr() const {
    return reinterpret_cast<char*>(ptr_);
  }

  size_t size() const {
    return size_;
  }

 private:
  void* ptr_;
  size_t size_;
};

}  // namespace gdt

#endif  // COMMON_BASE_SCOPED_MMAP_H_
