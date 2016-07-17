// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: Liu Xiaokang <hsiaokangliu@tencent.com>
// Modified By: Pei Xingxin <michaelpei@tencent.com>

#ifndef COMMON_SYSTEM_DIRECTORY_H_
#define COMMON_SYSTEM_DIRECTORY_H_

/// @brief direcotry operations.
/// @author hsiaokangliu
/// @author 2010-11-20

#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include <string>
#include <vector>

namespace gdt {

#define MAX_PATH_LEN 1024 * 10

namespace directory {
  bool Exists(const char* dir);
  bool Exists(const std::string& dir);
  bool IsDir(const char* path);
  bool IsDir(const std::string& dir);
  bool IsReadable(const char* dir);
  bool IsReadable(const std::string& dir);
  bool IsWritable(const char* dir);
  bool IsWritable(const std::string& dir);

  bool Create(const char* dir, int mode = 0755);
  bool Create(const std::string& dir, int mode = 0755);
  bool Delete(const char* dir);
  bool Delete(const std::string& dir);

  bool SetCurrentDir(const char* dir);
  bool SetCurrentDir(const std::string& dir);
  std::string GetCurrentDir();

  std::string ToWindowsFormat(const char* dir);
  std::string ToWindowsFormat(const std::string& dir);
  std::string ToUnixFormat(const char* dir);
  std::string ToUnixFormat(const std::string& dir);

  time_t GetAccessTime(const char* dir);
  time_t GetAccessTime(const std::string& dir);
  time_t GetCreateTime(const char* dir);
  time_t GetCreateTime(const std::string& dir);
  time_t GetLastModifyTime(const char* dir);
  time_t GetLastModifyTime(const std::string& dir);

  bool GetFiles(const char* dir, std::vector<std::string>* files);
  bool GetFiles(const std::string& dir, std::vector<std::string>* files);
  bool GetAllFiles(const char* dir, std::vector<std::string>* files);
  bool GetAllFiles(const std::string& dir, std::vector<std::string>* files);

  bool GetSubDirs(const char* dir, std::vector<std::string>* subdirs);
  bool GetSubDirs(const std::string& dir, std::vector<std::string>* subdirs);
  bool GetAllSubDirs(const char* dir, std::vector<std::string>* subdirs);
  bool GetAllSubDirs(const std::string& dir, std::vector<std::string>* subdirs);

  bool RecursiveDelete(const char* dir);
  bool RecursiveDelete(const std::string& dir);
}  // namespace directory

class DirectoryIterator {
 public:
  enum Flags {
    INVALID = 0x00,
    FILE = 0x01,
    DIRECTORY = 0x02,
    SYMBOLLINK = 0x04,
    ALL = FILE | DIRECTORY | SYMBOLLINK,
  };

 public:
  DirectoryIterator(
      const char* dir,
      int flags = ALL,
      const char* filter = NULL,
      int exclude_flags = INVALID);
  DirectoryIterator();
  ~DirectoryIterator() {
    Close();
  }

  bool Open(
      const std::string& dir,
      int flags = ALL,
      const std::string& filter = "",
      int exclude_flags = INVALID);

  std::string Name() const;
  std::string FullPath();
  bool Next();
  bool Close();
  bool IsEnd();
  int  GetType() const;

 private:
  void Initialize();
  bool SkipCurrent();
  DirectoryIterator(const DirectoryIterator&);
  DirectoryIterator& operator=(const DirectoryIterator&);

 private:
  DIR* dir_;
  struct dirent* dirent_;
  bool end_;
  int  flags_;
  int  exclude_flags_;
  std::string fopen_dir_;
  std::string filter_pattern_;
};  // class DirectoryIterator

}  // namespace gdt

#endif  // COMMON_SYSTEM_DIRECTORY_H_
