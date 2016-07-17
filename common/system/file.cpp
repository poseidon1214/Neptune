// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Modified By: Pei Xingxin <michaelpei@tencent.com>

// just rename file.cpp to common_file.cpp
// *.obj files in same directory on win32,
// there are file.cpp (./io/file.cpp, file.cpp)
// rename to support win32 build
//

#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "common/system/file.h"

#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include "common/system/directory.h"

namespace gdt {

namespace file {

bool Exists(const char* filename) {
  return IsRegular(filename);
}

bool Exists(const std::string& filename) {
  return Exists(filename.c_str());
}

bool IsRegular(const char* filename) {
  struct stat buf;
  if (!(stat(filename, &buf) == 0)) {
    return false;
  }
  return (buf.st_mode & S_IFREG) != 0;
}

bool IsRegular(const std::string& filename) {
  return IsRegular(filename.c_str());
}

bool GetSize(const char* filename, uint64_t* size) {
  struct stat buf;
  int ret = stat(filename, &buf);
  if (ret < 0) {
    return false;
  }

  *size = buf.st_size;
  return true;
}

bool GetSize(const std::string& filename, uint64_t* size) {
  return GetSize(filename.c_str(), size);
}

int64_t GetSize(const char* filename) {
  struct stat buf;
  int ret = stat(filename, &buf);
  if (ret < 0) {
    return -1;
  }
  return buf.st_size;
}

int64_t GetSize(const std::string& filename) {
  return GetSize(filename.c_str());
}

bool Copy(const char *src, const char* dest, bool overwrite) {
  FILE*  fp_src;
  FILE*  fp_dest;
  char   buffer[4096];

  if (!overwrite) {  // check if dest file exits when no over writing
    if (access(dest, F_OK) == 0) {   // file exists
      return false;
    } else if (errno != ENOENT) {
      // file doesn't exist, go on only when the file name is invaild
      return false;
    }
  }

  if ((fp_src = fopen(src, "rb")) == NULL) {
    return false;
  }
  if ((fp_dest = fopen(dest, "wb+")) == NULL) {
    fclose(fp_src);
    return false;
  }

  size_t ret = 0;
  while ((ret = fread(buffer, 1, sizeof(buffer), fp_src)) > 0) {
    if (fwrite(buffer, 1, ret, fp_dest) != ret) {
      fclose(fp_src);
      fclose(fp_dest);
      return false;
    }
  }
  fclose(fp_src);
  fclose(fp_dest);

  return true;
}

bool Copy(const std::string& src, const std::string& dest, bool overwrite) {
  return Copy(src.c_str(), dest.c_str(), overwrite);
}

bool Delete(const char* filename) {
  return remove(filename) == 0;
}

bool Delete(const std::string& filename) {
  return Delete(filename.c_str());
}

bool Rename(const char* oldname, const char* newname) {
  return rename(oldname, newname) == 0;
}

bool Rename(const std::string& oldname, const std::string& newname) {
  return Rename(oldname.c_str(), newname.c_str());
}

bool GetTime(const std::string& filename, FileTime* times) {
  struct stat buf;
  int ret = stat(filename.c_str(), &buf);
  if (ret < 0)
    return false;
  times->create_time_ = buf.st_ctime;
  times->access_time_ = buf.st_atime;
  times->modify_time_ = buf.st_mtime;

  return true;
}

bool SetTime(const std::string& filename, const FileTime& times) {
    utimbuf utimes = {};
    utimes.actime = times.access_time_;
    utimes.modtime = times.modify_time_;
    return utime(filename.c_str(), &utimes) == 0;
}

bool Touch(const std::string& filename) {
  int fd = open(filename.c_str(), O_WRONLY|O_NONBLOCK|O_CREAT|O_NOCTTY, 0666);
  if (fd >= 0) {
    close(fd);
    return utimes(filename.c_str(), NULL) == 0;
  }
  return false;
}

time_t GetAccessTime(const char *filename) {
  if (!IsRegular(filename)) {
    return -1;
  }
  struct stat buf;
  int ret = stat(filename, &buf);
  if (ret < 0) {
    return -1;
  }
  return buf.st_atime;
}

time_t GetAccessTime(const std::string& filename) {
  return GetAccessTime(filename.c_str());
}

time_t GetCreateTime(const char *filename) {
  if (!IsRegular(filename)) {
    return -1;
  }
  struct stat buf;
  int ret = stat(filename, &buf);
  if (ret < 0) {
    return -1;
  }
  return buf.st_ctime;
}

time_t GetCreateTime(const std::string& filename) {
  return GetCreateTime(filename.c_str());
}

time_t GetLastModifyTime(const char *filename) {
  if (!IsRegular(filename)) {
    return -1;
  }
  struct stat buf;
  int ret = stat(filename, &buf);
  if (ret < 0) {
    return -1;
  }
  return buf.st_mtime;
}

time_t GetLastModifyTime(const std::string& filename) {
  return GetLastModifyTime(filename.c_str());
}

bool IsReadable(const char* filename) {
  if (!Exists(filename)) {
    return false;
  }
  return access(filename, R_OK) == 0;
}

bool IsReadable(const std::string& filename) {
  return IsReadable(filename.c_str());
}

bool IsWritable(const char* filename) {
  if (!Exists(filename)) {
    return false;
  }
  return access(filename, W_OK) == 0;
}

bool IsWritable(const std::string& filename) {
  return IsWritable(filename.c_str());
}

bool ReadAll(const std::string& filename, void* buffer,
    size_t buffer_size, size_t* read_size) {
  int64_t size = GetSize(filename);
  if (size < 0)
    return false;

  if ((size_t)size > buffer_size)  // file too large
    return false;

  int fd = open(filename.c_str(), O_RDONLY);
  if (fd < 0)
    return false;

  *read_size = 0;
  while (*read_size < size) {
    int len = read(fd, static_cast<char*>(buffer) + (*read_size),
                   buffer_size - (*read_size));
    if (len < 0) {
      if (errno == EINTR) {
        continue;
      }
      close(fd);
      return false;
    }
    *read_size += len;
  }

  close(fd);
  return true;
}

template <typename T>
static bool ReadFileIntoContailer(const std::string& filename, T* content) {
  int64_t size = GetSize(filename);
  if (size < 0) {
    return false;
  }

  if ((size_t)size > SIZE_MAX) {
    // file too large
    return false;
  }

  int fd = open(filename.c_str(), O_RDONLY);
  if (fd < 0) {
    return false;
  }

  try {
    content->resize(size);
  } catch (std::bad_alloc& e) {
    close(fd);
    // file too large to fit into memory
    return false;
  }
  int64_t read_size = 0;
  while (read_size < size) {
    int64_t len = read(fd, &*content->begin() + read_size, size - read_size);
    if (len < 0) {
      if (errno == EINTR) {
        continue;
      }
      close(fd);
      return false;
    }
    read_size += len;
  }
  close(fd);
  return true;
}

bool ReadAll(const std::string& filename, std::string* content) {
  return ReadFileIntoContailer(filename, content);
}

bool ReadAll(const std::string& filename, std::vector<char>* content) {
  return ReadFileIntoContailer(filename, content);
}

static bool WriteFile(const std::string& filename, int flags,
                      const void* content, size_t size) {
  int fd = open(filename.c_str(), O_WRONLY | flags, 0666);
  if (fd < 0) {
    return false;
  }
  size_t write_size = 0;
  while (write_size < size) {
    int64_t len = write(fd, static_cast<const char*>(content) + write_size,
                        size - write_size);
    if (len < 0) {
      if (errno == EINTR) {
        continue;
      }
      close(fd);
      return false;
    }
    write_size += len;
  }
  close(fd);
  return true;
}

bool WriteTo(const std::string& filename, const std::string& content) {
  return WriteTo(filename, content.c_str(), content.size());
}

bool WriteTo(const std::string& filename, const std::vector<char>& content) {
  return WriteTo(filename, &*content.begin(), content.size());
}

bool WriteTo(const std::string& filename, const void* content, size_t size) {
  int flags = O_CREAT | O_TRUNC;
  return WriteFile(filename, flags, content, size);
}

bool AppendTo(const std::string& filename, const std::string& content) {
  return AppendTo(filename, content.c_str(), content.size());
}

bool AppendTo(const std::string& filename, const std::vector<char>& content) {
  return AppendTo(filename, &*content.begin(), content.size());
}

bool AppendTo(const std::string& filename, const void* content, size_t size) {
  int flags = O_APPEND;
  return WriteFile(filename, flags, content, size);
}

}  // namespace file
}  // namespace gdt
