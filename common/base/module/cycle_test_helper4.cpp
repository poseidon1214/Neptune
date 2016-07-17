// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/25/11
//
#include "common/base/module.h"

// namespace common {

// acyclic modules
DEFINE_MODULE(mod4) {
  return true;
}
USING_MODULE(mod3);

// } // namespace common
