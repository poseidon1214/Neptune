// Copyright (c) 2012, Tencent Inc. All rights reserved.
// Author: Liu Xiaokang <hsiaokangliu@tencent.com>

#include "common/system/path.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include <algorithm>
#include <string>
#include <vector>

#include "common/base/array_size.h"
#include "common/base/string/algorithm.h"

namespace gdt {

bool Path::IsSeparator(char ch) {
#ifdef _WIN32
  return ch == '\\' || ch == '/';
#else
  return ch == '/';
#endif
}

std::string Path::GetBaseName(StringPiece filepath) {
  size_t len = filepath.size();
  int i = static_cast<int>(len - 1);
  for (; i >= 0; i--) {
    if (IsSeparator(filepath[i]))
      break;
  }
  return std::string(filepath.data() + i + 1, filepath.data() + len);
}

std::string Path::GetExtension(StringPiece filepath) {
  size_t len = filepath.size();
  int i = static_cast<int>(len - 1);
  for (; i >= 0; i--) {
    if (filepath[i] == '.')
      return std::string(filepath.data() + i, filepath.data() + len);
    if (IsSeparator(filepath[i]))
      return "";
  }
  return "";
}

std::string Path::GetDirectory(StringPiece filepath) {
  size_t len = filepath.size();
  int i = static_cast<int>(len - 1);
  for (; i >= 0; i--) {
    if (IsSeparator(filepath[i]))
      break;
  }
  if (i >= 0)
    return std::string(filepath.data(), filepath.data() + i + 1);
  return "";
}

bool Path::IsAbsolute(StringPiece filepath) {
  return filepath.starts_with("/");
}

std::string Path::ToAbsolute(StringPiece filepath) {
  std::string path = IsAbsolute(filepath) ?
                     filepath.as_string() : Join(GetCurrentDir(), filepath);
  return Normalize(path);
}

// Normalize path, eliminating double slashes, etc.
std::string Path::Normalize(StringPiece path) {
  if (path.empty())
    return ".";

  int initial_slashes = StringStartsWith(path, "/");
  // POSIX allows one or two initial slashes, but treats three or more
  // as single slash.
  if (initial_slashes &&
      StringStartsWith(path, "//") && !StringStartsWith(path, "///")) {
    initial_slashes = 2;
  }

  std::vector<std::string> comps;
  SplitStringKeepEmpty(path, "/", &comps);

  std::vector<std::string> new_comps;
  for (std::vector<std::string>::iterator i = comps.begin(); i != comps.end();
       ++i) {
    const std::string& comp = *i;
    if (comp.empty() || comp == ".")
      continue;
    if (comp != ".." || (!initial_slashes && new_comps.empty()) ||
        (!new_comps.empty() && new_comps.back() == ".."))
      new_comps.push_back(comp);
    else if (!new_comps.empty())
      new_comps.pop_back();
  }

  std::swap(comps, new_comps);
  std::string new_path = JoinStrings(comps, "/");
  if (initial_slashes)
    new_path = std::string(initial_slashes, '/') + new_path;

  return new_path.empty() ? "." : new_path;
}

static std::string DoJoin(StringPiece** paths, size_t size) {
  std::string result = paths[0]->as_string();
  for (size_t i = 1; i < size; ++i) {
    StringPiece path = *paths[i];
    if (!path.empty()) {
      if (path[0] == '/') {
        result = path.as_string();
      } else {
        if (!result.empty() && result[result.size() - 1] != '/')
          result += '/';
        path.append_to_string(&result);
      }
    }
  }
  return result;
}

std::string Path::Join(StringPiece p1, StringPiece p2) {
  StringPiece* paths[] = {&p1, &p2};
  return DoJoin(paths, ARRAY_SIZE(paths));
}

std::string Path::Join(StringPiece p1, StringPiece p2, StringPiece p3) {
  StringPiece* paths[] = {&p1, &p2, &p3};
  return DoJoin(paths, ARRAY_SIZE(paths));
}

std::string Path::Join(StringPiece p1, StringPiece p2, StringPiece p3,
                       StringPiece p4) {
  StringPiece* paths[] = {&p1, &p2, &p3, &p4};
  return DoJoin(paths, ARRAY_SIZE(paths));
}

std::string Path::Join(StringPiece p1, StringPiece p2, StringPiece p3,
                       StringPiece p4, StringPiece p5) {
  StringPiece* paths[] = {&p1, &p2, &p3, &p4, &p5};
  return DoJoin(paths, ARRAY_SIZE(paths));
}

std::string Path::Join(StringPiece p1, StringPiece p2, StringPiece p3,
                       StringPiece p4, StringPiece p5, StringPiece p6) {
  StringPiece* paths[] = {&p1, &p2, &p3, &p4, &p5, &p6};
  return DoJoin(paths, ARRAY_SIZE(paths));
}

std::string Path::GetCurrentDir() {
  char path[PATH_MAX];
  char* p = getcwd(path, sizeof(path));
  // p may be NULL in some bad condition, for example, dir unlinked
  return p ? p : "";
}

bool Path::Exists(const std::string& path) {
  return access(path.c_str(), F_OK) == 0;
}

bool Path::IsDir(const std::string& path) {
  struct stat buf;
  if (stat(path.c_str(), &buf) < 0)
    return false;
  return S_ISDIR(buf.st_mode) != 0;
}

bool Path::IsFile(const std::string& path) {
  struct stat buf;
  if (stat(path.c_str(), &buf) < 0)
    return false;
  return S_ISREG(buf.st_mode) != 0;
}

#define RETURN_FILE_TIME(path, field) \
  struct stat buf; \
  if (stat(path, &buf) < 0) \
    return -1; \
  return buf.field;

time_t Path::GetAccessTime(const std::string& path) {
  RETURN_FILE_TIME(path.c_str(), st_atime);
}

time_t Path::GetChangeTime(const std::string& path) {
  RETURN_FILE_TIME(path.c_str(), st_ctime);
}

time_t Path::GetModifyTime(const std::string& path) {
  RETURN_FILE_TIME(path.c_str(), st_mtime);
}

}  // namespace gdt

