// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef FRAMEWORK_BITMAP_H_
#define FRAMEWORK_BITMAP_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"

namespace gdt {

// 设置第id位的bit值
inline void SetBit(int id, bool bit, BitMap* bitmap) {
  if (bitmap->bitset_size() <= id / 64) {
    bitmap->mutable_bitset()->Resize(id / 64 + 1, 0);
  }
  *(bitmap->mutable_bitset()->Mutable(id / 64)) = bitmap->bitset(id / 64) | (uint64_t(bit) << ((id - 1) % 64));
}
// 左值是否包含右值
inline bool Include(const BitMap& left, 
                    const BitMap& right) {
  for (size_t i = 0; i < right.bitset_size(); i++) {
    uint64_t left_bitset = (i < left.bitset_size()) ? left.bitset(i) : 0;
    if ((left_bitset | right.bitset(i)) != left_bitset) {
      return false;
    }
  }
  return true;
}
// 是否相等
inline bool operator == (const BitMap& left, 
                         const BitMap& right) {
  return Include(left, right) &&  Include(right, left);
}

inline bool operator & (const BitMap& left, 
                        const BitMap& right) {
  return true;
}



}  // namespace gdt
#endif  // FRAMEWORK_BITMAP_H_