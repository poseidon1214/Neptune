// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-2-26
#ifndef APP_QZAP_COMMON_UTILITY_FILE_UTILITY_H_
#define APP_QZAP_COMMON_UTILITY_FILE_UTILITY_H_

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include "common/base/uncopyable.h"

// man 3 glob for usage
// usually, glob_flags = 0 is good enough
bool Glob(
  const std::string &pattern,
  int32_t glob_flags,
  std::vector<std::string> * result);

bool IsDir(const std::string &file_path);

bool IsCFSFile(const std::string &abs_file_path);

// find files in all subdirs, including the dir itself
bool GlobInDirs(
  const std::string &pattern,
  int32_t glob_flags,
  std::vector<std::string> * result);

bool FileExisting(const std::string &file);

uint32_t ChecksumFile(const std::string &filename);

bool MD5sumFile(const std::string &filename, std::string *md5sum);
bool MD5sumFileWithLimitedSpeed(const std::string &filename,
                                std::string *md5sum,
                                uint32_t bytes_per_second);

// If the length is -1, means use the file content
// from offset to the end.
uint32_t ChecksumFileOffset(
  const std::string &filename,
  uint32_t offset, int32_t length);

bool CreateMultiLevelPath(
  const std::string &path,
  mode_t mode);

// The last six characters of template must be "XXXXXX" and these
// are replaced with a string that makes the filename unique.
// Note: this interface will change the content of path_template.
bool MakeUniquePath(char *path_template, mode_t mode);

bool GetLastModifyFile(const std::string &pattern, std::string *result);
off_t GetFileSize(const std::string &file);
bool MakesureFileExisting(const std::string &file_name, int flags);

bool CopyFile(const std::string &src_file, const std::string &dst_file);
bool CopyFileToCfs(const std::string &src_file,
                   const std::string &dst_file,
                   int speed_limit_k);

// length == -1 means that read the file from offset to the end
bool ReadFileOffsetToString(
  const std::string &filename,
  uint32_t offset, int32_t length,
  std::string *content);

bool ReadFileToStringWithLimitedSpeed(
  const std::string &filename,
  std::string *content,
  uint32_t bytes_per_second);

bool ReadFileOffsetToStringWithLimitedSpeed(
  const std::string &filename,
  uint32_t offset, int32_t length,
  std::string *content,
  uint32_t bytes_per_second);

bool ReadFileToString(
  const std::string &filename,
  std::string *content);

bool WriteStringToFileOffset(
  const std::string &filename,
  const std::string &content,
  uint32_t offset);

bool WriteStringToFileEnd(
  const std::string &filename,
  const std::string &content);

bool ReadStringFromFileEnd(
  const std::string &filename,
  std::string *content);

bool ReadStringFromFileTail(
  const std::string &filename,
  int line_number,
  std::vector<std::string> *content);

bool WriteStringToFile(
  const std::string &filename,
  const std::string &content);

bool SafeWriteStringToMD5File(
  const std::string &filename,
  const std::string &content,
  std::string *final_name = NULL);

bool SafeReadMD5FileToString(
  const std::string &filename,
  std::string *content);

// rename src_filename to dest_filename-md5sum(src_filename)
bool MD5RenameFile(
  const std::string &src_filename,
  const std::string &dest_filename,
  std::string *dest_filename_with_md5 = NULL);

// Sanity check filename(in format of *-md5sum).
bool MD5FileSanityCheck(const std::string &filename);
bool MD5FileSanityCheckWithLimitedSpeed(const std::string &filename,
                                        uint32_t bytes_per_second);

// Get the absolute path of the current execute path.
std::string GetCurrentExecutePath();
std::string GetCurrentExecuteDir();

std::string GetDirName(const std::string &path);
std::string GetBaseName(const std::string &path);

int64_t ReadN(int fd, void * buffer, int64_t size);
int64_t WriteN(int fd, const void * buffer, int64_t size);
// 获取替换后的文件名
std::string GetFormatFilename(const std::string& file_pattern,
                              uint32_t seconds_ago);
class AutoCloseFile {
 public:
  explicit AutoCloseFile(FILE* fd) : m_fd(fd) { }
  ~AutoCloseFile() {
    if (m_fd != NULL) {
      fclose(m_fd);
      m_fd = NULL;
    }
  }
  FILE* fd() {
    return m_fd;
  }
 private:
  FILE* m_fd;
  DECLARE_UNCOPYABLE(AutoCloseFile);
};
class AutoCloseFd {
 public:
  explicit AutoCloseFd(int32_t fd) : m_fd(fd) { }
  ~AutoCloseFd() {
    if (m_fd != -1) {
      close(m_fd);
      m_fd = -1;
    }
  }
  int32_t fd() {
    return m_fd;
  }
 private:
  int32_t m_fd;
  DECLARE_UNCOPYABLE(AutoCloseFd);
};
#endif  // APP_QZAP_COMMON_UTILITY_FILE_UTILITY_H_
