// Copyright (c) 2010, Tencent Inc. All rights reserved.
// Author: Liu Xiaokang <hsiaokangliu@tencent.com>

#ifndef COMMON_SYSTEM_PATH_H_
#define COMMON_SYSTEM_PATH_H_

#include <string>
#include "common/base/string/string_piece.h"

namespace gdt {

// File system path operation class
class Path {
 public:
  static bool IsSeparator(char ch);

  // Returns the final component of a pathname
  static std::string GetBaseName(StringPiece filepath);

  // Return extension name of path, include the leading dot.
  // Example:
  // "a.jpg" -> ".jpg"
  // "BUILD" -> ""
  static std::string GetExtension(StringPiece filepath);

  // Returns the directory components of a pathname
  static std::string GetDirectory(StringPiece filepath);

  // Return whether a path is absolute.
  static bool IsAbsolute(StringPiece filepath);

  // Convert to absolute path
  static std::string ToAbsolute(StringPiece filepath);

  // Normalize a path, e.g. A//B, A/./B and A/foo/../B all become A/B.
  // Take the same behavior as os.path.normpath of python.
  static std::string Normalize(StringPiece filepath);

  // Join two or more pathname components, inserting '/' as needed.
  // If any component is an absolute path, all previous path components
  // will be discarded.
  static std::string Join(StringPiece p1, StringPiece p2);
  static std::string Join(StringPiece p1, StringPiece p2, StringPiece p3);
  static std::string Join(StringPiece p1, StringPiece p2, StringPiece p3,
                          StringPiece p4);
  static std::string Join(StringPiece p1, StringPiece p2, StringPiece p3,
                          StringPiece p4, StringPiece p5);
  static std::string Join(StringPiece p1, StringPiece p2, StringPiece p3,
                          StringPiece p4, StringPiece p5, StringPiece p6);

  static std::string GetCurrentDir();

  static bool Exists(const std::string& path);
  static bool IsDir(const std::string& path);
  static bool IsFile(const std::string& path);

  // Return the last access time of a file
  static time_t GetAccessTime(const std::string& dir);
  // Return the metadata change time of a file
  static time_t GetChangeTime(const std::string& dir);
  // Return the last modification time of a file
  static time_t GetModifyTime(const std::string& dir);
};

}  // namespace gdt

#endif  // COMMON_SYSTEM_PATH_H_
