// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 09/30/11
// Description: blade static resource related interface

#ifndef COMMON_BASE_STATIC_RESOURCE_H_
#define COMMON_BASE_STATIC_RESOURCE_H_
#pragma once

#include "common/base/string/string_piece.h"

/// @brief convert a static array to static resource StringPiece
/// @param name name of the resource
///
/// example:
/// StringPiece icon = STATIC_RESOURCE(poppy_favicon_ico);
/// RegisterStaticResource("/favicon.ico", STATIC_RESOURCE(poppy_favicon_ico))
///
#define STATIC_RESOURCE(name) \
    StringPiece(RESOURCE_##name, sizeof(RESOURCE_##name))

// Blade generate struct
#ifndef BLADE_RESOURCE_TYPE_DEFINED
#define BLADE_RESOURCE_TYPE_DEFINED
struct BladeResourceEntry {
    const char* name;
    const char* data;
    unsigned int size;
};
#endif

namespace gdt {

// Each blade resource_library defines a static resource package,
// this class provide assess interface by file name.
class StaticResourcePackage {
 public:
  StaticResourcePackage(const BladeResourceEntry* entry, unsigned length);
  bool Find(StringPiece name, StringPiece* data) const;
 private:
  const BladeResourceEntry* resources_;
  unsigned int length_;
};

template <const BladeResourceEntry* entry, const unsigned int* length>
class ConstStaticResourcePackage : public StaticResourcePackage {
 private:
  ConstStaticResourcePackage() : StaticResourcePackage(entry, *length) {}
 public:
  static const StaticResourcePackage& Instance() {
    // Mayers singleton
    static const ConstStaticResourcePackage package;
    return package;
  }
};

// Static resource package access macro
#define STATIC_RESOURCE_PACKAGE(name) \
    ::gdt::ConstStaticResourcePackage<RESOURCE_INDEX_##name, \
                                      &RESOURCE_INDEX_##name##_len>::Instance()

}  // namespace gdt

#endif  // COMMON_BASE_STATIC_RESOURCE_H_
