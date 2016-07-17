// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_BASE_GFLAGS_IN_SPP_H_
#define COMMON_BASE_GFLAGS_IN_SPP_H_
#pragma once

namespace gdt {

// Init gflags in shared library loaded by spp.
// should be called in spp_handle_init.
// etc: the first parameter of spp_handle_init,
// is also the value of etc attribute in plugin xml.
//
// Example:
// extern "C" int spp_handle_init(void* etc, void* arg) {
//   gdt::InitGflagsInSpp(etc);
//   // ...
//   return 0;
// }
//
void InitGflagsInSpp(const void* etc);

}  // namespace gdt

#endif  // COMMON_BASE_GFLAGS_IN_SPP_H_
