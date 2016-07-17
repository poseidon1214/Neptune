// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#ifndef COMMON_NET_HTTP_TRANSFER_H_
#define COMMON_NET_HTTP_TRANSFER_H_
#pragma once

#include <stddef.h>
#include "common/base/string/string_piece.h"

namespace gdt {

const size_t kMaxHttpHeadersSize = 4096;

// Return the http header size detected.
// Return 0 if can't obtain yet
// Return -1 if error detected.
int DetectHttpHeaderSize(StringPiece buffer);

}  // namespace gdt

#endif  // COMMON_NET_HTTP_TRANSFER_H_
