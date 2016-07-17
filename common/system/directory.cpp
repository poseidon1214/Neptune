// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: Liu Xiaokang <hsiaokangliu@tencent.com>
// Modified by: Pei Xingxin <michaelpei@tencent.com>

#include "common/system/directory.h"

#include <fnmatch.h>
#include <stdlib.h>
#include <set>
#include <string>
#include <vector>

#include "common/system/file.h"
#include "common/system/path.h"

namespace gdt {
const char kPathSep = '/';

void DirectoryIterator::Initialize() {
  dir_ = NULL;
  dirent_ = NULL;
  end_ = true;
  flags_ = ALL;
  exclude_flags_ = INVALID;
}

DirectoryIterator::DirectoryIterator() {
  Initialize();
}

DirectoryIterator::DirectoryIterator(const char* dir, int flags,
                                     const char* filter, int exclude_flags) {
  Initialize();
  Open(dir, flags, filter, exclude_flags);
}

bool DirectoryIterator::SkipCurrent() {
  if (dirent_ == NULL)
    return false;
  // skip "." and ".." directory
  if (strncmp(dirent_->d_name, ".", MAX_PATH_LEN) == 0 ||
      strncmp(dirent_->d_name, "..", MAX_PATH_LEN) == 0)
    return true;

  if (!filter_pattern_.empty() && !fnmatch(filter_pattern_.c_str(),
        dirent_->d_name, 0))
    return true;
  int type = GetType();
  if ((type == INVALID) ||
      ((type & flags_) == 0) ||
      ((type & exclude_flags_) != 0)) {
    return true;
  }
  return false;
}

int DirectoryIterator::GetType() const {
  if (end_) {
    return INVALID;
  }
  int type = INVALID;

  std::string path = fopen_dir_;
  path += "/";
  path += dirent_->d_name;
  struct stat buf;
  int ret = lstat(path.c_str(), &buf);
  if (ret < 0) {
    return type;
  }
  if (S_ISDIR(buf.st_mode))
    type |= DIRECTORY;
  if (S_ISREG(buf.st_mode))
    type |= FILE;
  if (S_ISLNK(buf.st_mode))
    type |= SYMBOLLINK;
  return type;
}

bool DirectoryIterator::Open(const std::string& dir, int flags,
                             const std::string& filter, int exclude_flags) {
  flags_ = flags;
  exclude_flags_ = exclude_flags;
  fopen_dir_ = dir;
  filter_pattern_ = filter;

  if (dir_) {
    closedir(dir_);
  }
  dir_ = opendir(dir.c_str());
  if (!dir_) {
    return false;
  }
  end_ = false;
  Next();

  return true;
}

bool DirectoryIterator::IsEnd() {
  return end_;
}

std::string DirectoryIterator::Name() const {
  if (end_) {
    return "";
  }
  return dirent_->d_name;
}

std::string DirectoryIterator::FullPath() {
  std::string filename = Name();
  if (filename.empty())
    return filename;
  std::string related_path = fopen_dir_;
  related_path += kPathSep;
  related_path += filename;
  filename = Path::ToAbsolute(related_path);
  return filename;
}

bool DirectoryIterator::Next() {
  if (dir_) {
    dirent_ = readdir(dir_);
    while (dirent_ != NULL && SkipCurrent()) {
      dirent_ = readdir(dir_);
    }
    if (dirent_ == NULL) {
      end_ = true;
    }
    return dirent_ != NULL;
  }

  return false;
}

bool DirectoryIterator::Close() {
  if (dir_) {
    closedir(dir_);
    dir_ = NULL;
  }
  end_ = true;
  return true;
}

namespace directory {

bool IsDir(const char* dir) {
  struct stat buf;
  if (!(stat(dir, &buf) == 0)) {
    return false;
  }
  return (buf.st_mode & S_IFDIR) != 0;
}

bool IsDir(const std::string& dir) {
  return Exists(dir.c_str());
}

bool Exists(const char* dir) {
  return IsDir(dir);
}

bool Exists(const std::string& dir) {
  return IsDir(dir);
}

bool IsReadable(const char* dir) {
  if (!Exists(dir)) {
    return false;
  }
  return access(dir, R_OK) == 0;
}

bool IsReadable(const std::string& dir) {
  return IsReadable(dir.c_str());
}

bool IsWritable(const char* dir) {
  if (!Exists(dir)) {
    return false;
  }
  return access(dir, W_OK) == 0;
}

bool IsWritable(const std::string& dir) {
  return IsWritable(dir.c_str());
}

bool Create(const char* dir, int mode) {
  return (mkdir(dir, mode) == 0);
}

bool Create(const std::string& dir, int mode) {
  return Create(dir.c_str(), mode);
}

bool Delete(const char *dir) {
  if (!Exists(dir)) {
    return false;
  }
  return rmdir(dir) == 0;
}

bool Delete(const std::string& dir) {
  return Delete(dir.c_str());
}

std::string ToWindowsFormat(const char* dir) {
  std::string str(dir);
  size_t len = str.length();
  for (size_t idx = 0; idx < len; idx++) {
    if (str[idx] == '/') {
      str[idx] = '\\';
    }
  }
  return str;
}

std::string ToWindowsFormat(const std::string& dir) {
  return ToWindowsFormat(dir.c_str());
}

std::string ToUnixFormat(const char* dir) {
  std::string str(dir);
  size_t len = str.length();
  for (size_t idx = 0; idx < len; idx++) {
    if (str[idx] == '\\') {
      str[idx] = '/';
    }
  }
  return str;
}

std::string ToUnixFormat(const std::string& dir) {
  return ToUnixFormat(dir.c_str());
}

bool SetCurrentDir(const char *dir) {
  if (!Exists(dir)) {
    return false;
  }

  return (chdir(dir) == 0);
}

bool SetCurrentDir(const std::string& dir) {
  return SetCurrentDir(dir.c_str());
}

std::string GetCurrentDir() {
  char path[MAX_PATH_LEN];
  char* p = getcwd(path, MAX_PATH_LEN);
  // p may be NULL in some bad condition, for example, dir unlinked
  return p ? p : "";
}

time_t GetAccessTime(const char* dir) {
  if (!IsDir(dir)) {
    return -1;
  }
  struct stat buf;
  int ret = stat(dir, &buf);
  if (ret < 0) {
    return -1;
  }
  return buf.st_atime;
}

time_t GetAccessTime(const std::string& dir) {
  return GetAccessTime(dir.c_str());
}

time_t GetCreateTime(const char* dir) {
  if (!IsDir(dir)) {
    return -1;
  }
  struct stat buf;
  int ret = stat(dir, &buf);
  if (ret < 0) {
    return -1;
  }
  return buf.st_ctime;
}

time_t GetCreateTime(const std::string& dir) {
  return GetCreateTime(dir.c_str());
}

time_t GetLastModifyTime(const char* dir) {
  if (!IsDir(dir)) {
    return -1;
  }
  struct stat buf;
  int ret = stat(dir, &buf);
  if (ret < 0) {
    return -1;
  }
  return buf.st_mtime;
}

time_t GetLastModifyTime(const std::string& dir) {
  return GetLastModifyTime(dir.c_str());
}

bool GetFiles(const char* dir, std::vector<std::string>* files) {
  files->clear();
  DirectoryIterator iter;
  if (!iter.Open(dir, DirectoryIterator::FILE)) {
    return false;
  }
  while (!iter.IsEnd()) {
    std::string filename = iter.Name();
    std::string path = dir;
    path += kPathSep;
    path += filename;
    files->push_back(path);
    iter.Next();
  }
  iter.Close();

  return true;
}

bool GetFiles(const std::string& dir, std::vector<std::string>* files) {
  return GetFiles(dir.c_str(), files);
}

bool GetAllFiles(const char* dir, std::vector<std::string>* files) {
  files->clear();
  bool ret = GetFiles(dir, files);
  std::set<std::string> visited_dirs;
  std::string fullpath = Path::ToAbsolute(dir);
  visited_dirs.insert(fullpath);

  std::vector<std::string> subdirs;
  ret = ret && GetAllSubDirs(dir, &subdirs);
  std::vector<std::string> subfiles;
  for (size_t i = 0; i < subdirs.size(); i++) {
    fullpath = Path::ToAbsolute(subdirs[i].c_str());
    if (visited_dirs.find(fullpath) != visited_dirs.end()) {
      continue;
    }
    visited_dirs.insert(fullpath);
    ret = ret && GetFiles(subdirs[i], &subfiles);
    files->insert(files->end(), subfiles.begin(), subfiles.end());
  }
  return ret;
}

bool GetAllFiles(const std::string& dir, std::vector<std::string>* files) {
  return GetAllFiles(dir.c_str(), files);
}

bool GetSubDirs(const char* dir, std::vector<std::string>* subdirs) {
  subdirs->clear();
  DirectoryIterator iter;
  if (!iter.Open(dir, DirectoryIterator::DIRECTORY)) {
    return false;
  }
  while (!iter.IsEnd()) {
    std::string filename = iter.Name();
    std::string path = dir;
    path += kPathSep;
    path += filename;
    subdirs->push_back(path);
    iter.Next();
  }
  iter.Close();
  return true;
}

bool GetSubDirs(const std::string& dir, std::vector<std::string>* subdirs) {
    return GetSubDirs(dir.c_str(), subdirs);
}

bool GetAllSubDirs(const char* dir, std::vector<std::string>* subdirs) {
  subdirs->clear();
  bool ret = GetSubDirs(dir, subdirs);
  std::set<std::string> visited_dirs;
  std::string fullpath = Path::ToAbsolute(dir);
  visited_dirs.insert(fullpath);

  size_t idx = 0;
  while (idx < subdirs->size()) {
    fullpath = Path::ToAbsolute(subdirs->at(idx).c_str());
    if (visited_dirs.find(fullpath) == visited_dirs.end()) {
      std::vector<std::string> vdirs;
      ret = ret && GetSubDirs(subdirs->at(idx), &vdirs);
      subdirs->insert(subdirs->end(), vdirs.begin(), vdirs.end());
      visited_dirs.insert(fullpath);
    }
    idx++;
  }
  return ret;
}

bool GetAllSubDirs(const std::string& dir, std::vector<std::string>* subdirs) {
  return GetAllSubDirs(dir.c_str(), subdirs);
}

bool RecursiveDelete(const char* dir) {
  DirectoryIterator iter;
  bool ret = iter.Open(dir);
  if (!ret) {
    return false;
  }
  while (!iter.IsEnd()) {
    std::string filepath = dir;
    filepath += kPathSep;
    filepath += iter.Name();
    if ((iter.GetType() & (DirectoryIterator::FILE |
            DirectoryIterator::SYMBOLLINK)) != 0) {
      ret = ret && file::Delete(filepath);
    } else {
      ret = ret && RecursiveDelete(filepath);
    }
    iter.Next();
  }
  directory::Delete(dir);
  iter.Close();
  return ret;
}

bool RecursiveDelete(const std::string& dir) {
  return RecursiveDelete(dir.c_str());
}

}  // namespace directory

}  // namespace gdt
