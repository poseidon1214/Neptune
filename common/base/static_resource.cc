// Copyright (c) 2015, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "common/base/static_resource.h"

namespace gdt {

StaticResourcePackage::StaticResourcePackage(
    const BladeResourceEntry* entry, unsigned length)
  : resources_(entry), length_(length) {
}

bool StaticResourcePackage::Find(StringPiece name, StringPiece* data) const {
  for (unsigned int i = 0; i < length_; ++i) {
    if (resources_[i].name == name) {
      data->assign(resources_[i].data, resources_[i].size);
      return true;
    }
  }
  return false;
}

}  // namespace gdt
