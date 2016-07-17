// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/25/11
// Description: test cycle dependancy

#include "common/base/module.h"

// namespace common {

// cyclic module
DEFINE_MODULE(mod2) {
  return true;
}
USING_MODULE(mod1);

// } // namespace common
