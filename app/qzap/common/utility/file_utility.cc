// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-2-26
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <glob.h>
#include <libgen.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <string>
#include <vector>
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "app/qzap/common/utility/time_utility.h"
#include "app/qzap/common/base/walltime.h"
#include "thirdparty/openssl/md5.h"
static int GlobErrFunc(
    const char *path,
    int errorno) {
  // ENOENT is "file not found".
  // EACCES is "permission denied"
  return (errorno == ENOENT || errorno == EACCES) ? 0 : 1;
}

bool Glob(
    const std::string &pattern,
    int32_t glob_flags,
    std::vector<std::string> *files) {
  glob_t g;

  if (files == NULL) {
    LOG(ERROR) << "Match passed NULL for list for pattern " << pattern;
    return false;
  }

  int result = glob(pattern.c_str(), glob_flags, GlobErrFunc, &g);
  if (result == 0) {
    for (size_t i = 0; i < g.gl_pathc; i++) {
      // Insert only if the pathname is not empty since 'glob' return
      // '' as a matched path if pattern is an empty std::string.
      if (strlen(g.gl_pathv[i]) != 0)
        files->push_back(std::string(g.gl_pathv[i]));
    }
  }
  globfree(&g);
  return result == 0 || result == GLOB_NOMATCH;
}

bool IsDir(const std::string &file_path) {
  struct stat buf;
  int ret = stat(file_path.c_str(), &buf);
  if (ret != 0) {
    return false;
  }

  return (S_ISDIR(buf.st_mode));
}
bool IsCFSFile(const std::string &abs_file_path) {
  return abs_file_path.size() >= 4u && abs_file_path.substr(0, 4) == "/cfs";
}

bool GlobInDirs(
    const std::string &pattern,
    int32_t glob_flags,
    std::vector<std::string> *result) {
  char *dirc = strdup(pattern.c_str());
  char *basec = strdup(pattern.c_str());
  std::string base = basename(basec);
  std::string dir  = dirname(dirc);
  free(dirc);
  free(basec);
  struct dirent   entry;
  struct dirent   *result_entry;
  DIR             *dp;

  std::string file_pattern = dir + "/" + base;
  if (!Glob(file_pattern, glob_flags, result)) {
    return false;
  }
  if ((dp = opendir(dir.c_str())) == NULL) {
    LOG(ERROR) << "opendir for dir " << dir.c_str() << " error:" << strerror(errno);
    return false;
  }
  std::string dirname;
  struct stat buf;
  while (readdir_r(dp, &entry, &result_entry) == 0 &&
         result_entry != NULL) {
    dirname = dir + "/" + result_entry->d_name;
    if (result_entry->d_name[0] != '.' &&     // ignore . and ..
        stat(dirname.c_str(), &buf) == 0 &&
        S_ISDIR(buf.st_mode)) {
      file_pattern = dirname + "/" + base;
      if (!GlobInDirs(file_pattern, glob_flags, result)) {
        break;
      }
    }
  }
  if (closedir(dp) < 0) {
    LOG(ERROR) << "closedir error: " << strerror(errno);
    return -1;
  }

  return result_entry == NULL;
}

bool FileExisting(const std::string &filename) {
  return 0 == access(filename.c_str(), F_OK);
}

uint32_t ChecksumFile(const std::string &filename) {
  return ChecksumFileOffset(filename, 0, -1);
}

uint32_t ChecksumFileOffset(const std::string &filename,
    uint32_t offset, int32_t length) {
  std::string content;
  if (!ReadFileOffsetToString(filename,
      offset, length, &content)) {
    return false;
  }
  return ChecksumString(content);
}

bool CreateMultiLevelPath(const std::string &path, mode_t mode) {
  int32_t len = path.size();
  scoped_array<char> local_path(new char[len + 1]);
  if (local_path == NULL) {
    LOG(ERROR) << "memory allocation fail";
    return false;
  }
  strncpy(local_path.get(), path.c_str(), len);
  local_path[len] = '\0';
  int ret = 0;
  for (int i = 0; i < len; ++i) {
    if (i == 0 && local_path[i] == '/') {
      continue;
    }
    if (local_path[i] == '/') {
      local_path[i] = '\0';
      if (!FileExisting(local_path.get())) {
        ret = mkdir(local_path.get(), mode);
        if (ret != 0 && errno != EEXIST) {
          LOG(ERROR) << strerror(errno) << " path:" << local_path.get();
          return false;
        }
      }
      local_path[i] = '/';
    }
  }
  if (len > 0 && !FileExisting(local_path.get())) {
    ret = mkdir(local_path.get(), mode);
    if (ret != 0 && errno != EEXIST) {
      VLOG(2) << strerror(errno) << " path:" << local_path.get();
      return false;
    }
  }
  return true;
}

bool MakeUniquePath(char *path_template, mode_t mode) {
  std::string path(path_template);
  char *dir = dirname(&path[0]);
  VLOG(2) << "dir: " << dir;
  bool ret;
  int fail_count = 0;
  do {
    if (++fail_count > 3) {
      LOG(WARNING) << "MakeUniquePath " << path_template << " with mode "
          << mode << " failed";
      return false;
    }
    ret = CreateMultiLevelPath(dir, mode);
  } while (!ret);

  int fd = mkstemp(path_template);
  if (fd == -1) {
    LOG(ERROR) << "mkstemp " << path_template << " error, errno: " << errno << " : " << strerror(errno);
    return false;
  }
  unlink(path_template);
  close(fd);
  return true;
}

bool GetLastModifyFile(const std::string &pattern, std::string *result) {
  time_t cur_tm = 0;
  int32_t last_idx = 0;
  std::vector<std::string> v_str;
  if (!result) {
    return false;
  }
  if (!Glob(pattern, 0, &v_str)) {
    return false;
  }
  for (size_t i = 0; i < v_str.size(); ++i) {
    struct stat info;
    if (stat(v_str[i].c_str(), &info) != -1) {
      if (S_ISREG(info.st_mode) && info.st_ctime > cur_tm) {
        cur_tm = info.st_ctime;
        last_idx = i;
      }
    }
  }
  *result = v_str[last_idx];
  return true;
}

off_t GetFileSize(const std::string &file) {
  struct stat st;
  if (stat(file.c_str(), &st) == 0) {
    return st.st_size;
  }
  return -1;
}

bool MakesureFileExisting(const std::string &file_name, int flags) {
  if (FileExisting(file_name)) {
    return true;
  }
  std::string dir_name = file_name;
  char *file_dir = dirname(&dir_name[0]);
  if (!CreateMultiLevelPath(file_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
    LOG(ERROR) << "Create path of : " << file_name << " fail.";
    return false;
  }
  int fd = open(file_name.c_str(), O_CREAT|O_RDONLY, flags);
  if (fd < 0) {
    LOG(ERROR) << "Fail to open file: " << file_name << ": " << strerror(errno);
    return false;
  }
  if (close(fd) != 0) {
    char error_msg[1024];
    LOG(ERROR) << "Fail to close file, filename:" << file_name << " error_msg:" << strerror_r(errno, error_msg, sizeof(error_msg));
    return false;
  }
  else {
    return true;
  }
}

bool ReadFileToString(const std::string &filename, std::string *content) {
  return ReadFileOffsetToString(filename, 0, -1, content);
}

bool ReadFileToStringWithLimitedSpeed(
  const std::string &filename,
  std::string *content,
  uint32_t bytes_per_second) {
  return ReadFileOffsetToStringWithLimitedSpeed(filename, 0, -1, content, bytes_per_second);
}

bool ReadFileOffsetToString(
    const std::string &filename,
    uint32_t offset, int32_t length,
    std::string *content) {
  return ReadFileOffsetToStringWithLimitedSpeed(filename, offset, length, content, 0u);
}
bool ReadFileOffsetToStringWithLimitedSpeed(
  const std::string &filename,
  uint32_t offset, int32_t length,
  std::string *content,
  uint32_t bytes_per_second) {
  if (length == 0)
    return true;

  FILE *f = fopen(filename.c_str(), "rb");
  if (f == NULL) {
    LOG(ERROR) << "open failed:" << filename << ','
               << errno << ',' << strerror(errno);
    return false;
  }

  if (offset > 0) {
    int ret = fseek(f, offset, SEEK_SET);
    if (ret < 0) {
      LOG(ERROR) << "fseek failed, offset=" << offset << ','
                 << errno << ',' << strerror(errno);
      fclose(f);
      return false;
    }
  }

  char buf[4096] = {0};
  int leave_size = length;
  int readed_size = 0;
  double begin_time = WallTime_Now();
  while (true) {
    int expect_size = sizeof(buf);
    if (length != -1 && leave_size < expect_size) {
      expect_size = leave_size;
    }

    int readed = fread(buf, 1, expect_size, f);
    DCHECK_GE(readed, 0);
    content->append(buf, readed);
    if (readed < expect_size) {
      break;
    }

    if (length != -1) {
      leave_size -= readed;
      DCHECK_GE(leave_size, 0);
      if (leave_size == 0)
        break;
    }
    readed_size += readed;
    if (bytes_per_second > 0u) {
      double time_cost = WallTime_Now() - begin_time;
      if (time_cost > 0.00001 && (time_cost * bytes_per_second) < readed_size) {
          double sleep_time_in_second = static_cast<double>(readed_size) / bytes_per_second  - time_cost;
          if (sleep_time_in_second > 0.00001) {
            SleepForSeconds(sleep_time_in_second);
          }
      }
    }
  }

  fclose(f);
  return true;
}

bool WriteStringToFileOffset(
    const std::string &filename,
    const std::string &content,
    uint32_t offset) {
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open(filename.c_str(), O_WRONLY | O_CREAT, mode);
  if (fd < 0) {
    LOG(ERROR) << "open file " << filename
               << " error: " << strerror(errno);
    return false;
  }
  const char *p = content.c_str();
  size_t size = content.size();
  while (true) {
    int n = pwrite(fd, p, size, offset);
    if (n == static_cast<int>(size)) {
      if (close(fd) != 0) {
        char error_msg[1024];
        LOG(ERROR) << "Fail to close file, filename:" << filename << " error_msg:" << strerror_r(errno, error_msg, sizeof(error_msg));
        return false;
      }
      else {
        return true;
      }
    }
    if (errno == EAGAIN) {
      p += n;
      offset += n;
      size -= n;
      continue;
    }
    LOG(ERROR) << "Write fail, : error: " << strerror(errno);
    close(fd);
    return false;
  }
}

bool WriteStringToFile(const std::string &filename, const std::string &content) {
  return WriteStringToFileOffset(filename, content, 0);
}

bool WriteStringToFileEnd(
    const std::string &filename,
    const std::string &content) {
  FILE *fp = fopen(filename.c_str(), "a");
  if (fp == NULL) {
    LOG(ERROR) << "fopen file " << filename
               << " error: " << strerror(errno);
    return false;
  }

  const char *p = content.c_str();
  size_t size = content.size();
  while (true) {
    int n = fwrite(p, 1, size, fp);
    if (n == static_cast<int>(size)) {
      if (fclose(fp) != 0) {
        char error_msg[1024];
        LOG(ERROR) << "Fail to close file, filename:" << filename << " error_msg:" << strerror_r(errno, error_msg, sizeof(error_msg));
        return false;
      }
      else {
        return true;
      }
    }
    if (errno == EAGAIN) {
      p += n;
      size -= n;
      continue;
    }
    fclose(fp);
    return false;
  }
}

bool ReadStringFromFileEnd(
  const std::string &filename,
  std::string *content) {
  std::vector<std::string> c;
  if (ReadStringFromFileTail(filename, 1, &c)) {
    *content = c[0];
    return true;
  }
  return false;
}

static inline void SearchLine(char *buffer, int size, char line_delimiter, std::string *left, std::vector<std::string> *lines, const size_t line_number) {
  int j = size - 1;
  int i = size - 1;
  for (; i >= 0; --i) {
    if (buffer[i] == line_delimiter) {
      // move i to size to tmp, move tmp to lines.
      while (j > i && line_delimiter == buffer[j]) --j;
      for (; j > i; --j) {
        left->push_back(buffer[j]);
      }
      if (!left->empty() && (lines->size() < line_number)) {
        std::reverse(&(*left)[0], &(*left)[0] + left->size());
        lines->push_back(*left);
        left->clear();
        if (lines->size() == line_number) {
          return;
        }
      }
    }
  }
  if (j > 0) {
    for (; j >=0 && line_delimiter == buffer[j]; --j);
    for (i = 0; i < j && line_delimiter == buffer[i]; ++i);
    for (; j >= i; --j) {
      left->push_back(buffer[j]);
    }
  }
}

bool ReadStringFromFileTail(
    const std::string &filename,
    int line_number,
    std::vector<std::string> *content) {
  static const int kSearchLineLength = 1024;
  static const char kDelimiter = '\n';
  FILE *fp = fopen(filename.c_str(), "rb");
  if (fp == NULL) {
    LOG(ERROR) << "fopen file " << filename
               << " error: " << strerror(errno);
    return false;
  }

  int ret = fseek(fp, 0, SEEK_END);
  if (ret != 0) {
    LOG(ERROR) << "fseek " << filename << " error";
    fclose(fp);
    return false;
  }
  off_t filesize = ftell(fp);
  off_t offset = filesize; // skip the \n
  char buffer[kSearchLineLength] = { 0 };
  std::string left;
  for (;;) {
    int buffer_size = sizeof(buffer);
    if (offset >= buffer_size) {
      offset -= buffer_size;
    } else {
      buffer_size = offset;
      offset = 0;
    }
    ret = fseek(fp, offset, SEEK_SET);
    if (ret < 0) {
      LOG(ERROR) << "fseek failed, offset=" << offset
        << errno << "," << strerror(errno);
      fclose(fp);
      return false;
    }
    int readed = fread(buffer, 1, buffer_size, fp);
    if (readed < 0) {
      LOG(ERROR) << "fread failed: " << errno << ", " << strerror(errno);
      fclose(fp);
      return false;
    }
    SearchLine(buffer, readed, kDelimiter, &left, content, line_number);
    if (static_cast<int>(content->size()) == line_number) {
      break;
    }
    if (offset == 0) {
      if (static_cast<int>(content->size()) < line_number && !left.empty()) {
        std::reverse(&left[0], &left[0] + left.size());
        content->push_back(left);
      }
      break;
    }
  }
  std::reverse(content->begin(), content->end());
  fclose(fp);
  return true;
}

bool SafeWriteStringToMD5File(
  const std::string &filename,
  const std::string &content,
  std::string *final_name) {
  std::string md5_filename = filename + "-" + MD5sumString(content);
  std::string dir_name = GetDirName(filename);
  std::string tmp_file = dir_name + "/.safewritestringtomd5file.XXXXXX";
  if (!FileExisting(dir_name)) {
    if (!CreateMultiLevelPath(dir_name,  0755)) {
      LOG(ERROR) << "SafeWriteStringToMD5File fail to create multilevel path: " << dir_name;
      return false;
    }
  }
  int fd = mkstemp(&tmp_file[0]);
  if (fd == -1) {
    LOG(ERROR) << "mkstemp " << tmp_file << " error, errno: " << errno << " : " << strerror(errno);
    return false;
  }
  if (close(fd) != 0) {
    char error_msg[1024];
    LOG(ERROR) << "Fail to close file, filename:" << tmp_file << " error_msg:" << strerror_r(errno, error_msg, sizeof(error_msg));
    return false;
  }

  if (!WriteStringToFile(tmp_file, content)) {
    LOG(ERROR) << "WriteStringToFile fail";
    return false;
  }
  if (0 != rename(tmp_file.c_str(), md5_filename.c_str())) {
    LOG(ERROR) << "rename : " << tmp_file.c_str() << " to: " << md5_filename.c_str()
      << " fail, error: " << strerror(errno);
    return false;
  }
  if (final_name != NULL) {
    *final_name = md5_filename;
  }
  return true;
}

bool SafeReadMD5FileToString(
  const std::string &filename,
  std::string *content) {
  std::string tmp;
  if (!ReadFileToString(filename, &tmp)) {
    LOG(ERROR) << "Fail to ReadFileToString";
    return false;
  }
  std::vector<std::string> field;
  if (!SplitString(filename, "-", &field)) {
    LOG(ERROR) << "Fail to split filename: " << filename;
    return false;
  }
  if (MD5sumString(tmp) != *field.rbegin()) {
    LOG(ERROR) << "MD5sumString not equal: " << MD5sumString(tmp)
     << " : " << *field.rbegin();
    return false;
  }
  *content = tmp;
  return true;
}

bool MD5sumFile(const std::string &filename, std::string *md5sum) {
    return MD5sumFileWithLimitedSpeed(filename, md5sum, 0u);
};
bool MD5sumFileWithLimitedSpeed(const std::string &filename, std::string *md5sum, uint32_t bytes_per_second) {
  FILE *f = fopen(filename.c_str(), "rb");
  if (f == NULL) {
    LOG(ERROR) << "open failed:" << filename << ','
               << errno << ',' << strerror(errno);
    return false;
  }

  static const int buf_length = 4096;
  scoped_array<char> buf(new char[buf_length]);
  MD5_CTX md5_hash;
  MD5_Init(&md5_hash);
  int readed_size = 0;
  double begin_time = WallTime_Now();
  while (true) {
    int readed = fread(buf.get(), 1, buf_length, f);
    MD5_Update(&md5_hash, buf.get(), readed);
    if (readed < buf_length) {
      break;
    }
    readed_size += readed;
    if (bytes_per_second > 0u) {
      double time_cost = WallTime_Now() - begin_time;
      if (time_cost > 0.00001 && (time_cost * bytes_per_second) < readed_size) {
          double sleep_time_in_second = static_cast<double>(readed_size) / bytes_per_second  - time_cost;
          if (sleep_time_in_second > 0.00001) {
            SleepForSeconds(sleep_time_in_second);
          }
      }
    }
  }
  unsigned char md[MD5_DIGEST_LENGTH];
  MD5_Final(md, &md5_hash);
  fclose(f);
  static const char kLookupHex[] = "0123456789abcdef";
  std::string ret;
  ret.resize(arraysize(md) * 2);
  for (int i = 0; i < arraysize(md); ++i) {
    const int lo = md[i] & 0xF;
    const int hi = (md[i] >> 4) & 0xF;
    ret[2 * i] = kLookupHex[hi];
    ret[2 * i + 1] = kLookupHex[lo];
  }
  *md5sum = ret;
  return true;
}

bool MD5RenameFile(
  const std::string &src_filename,
  const std::string &dest_filename,
  std::string *dest_filename_with_md5) {
  std::string md5sum;
  if (!MD5sumFile(src_filename, &md5sum)) {
    LOG(ERROR) << "MD5sumFile : " << src_filename << " fail";
    return false;
  }
  std::string filename = dest_filename + "-" + md5sum;
  if (!MakesureFileExisting(filename, 0755)) {
    LOG(ERROR) << "MD5RenameFile MakesureFileExisting fail: " << filename;
    return false;
  }
  if (0 != rename(src_filename.c_str(), filename.c_str())) {
    LOG(ERROR) << "rename : " << src_filename.c_str() << " to: "
      << filename.c_str()
      << " fail, error: " << strerror(errno);
    return false;
  }
  if (dest_filename_with_md5 != NULL) {
    *dest_filename_with_md5 = filename;
  }
  return true;
}

bool MD5FileSanityCheck(const std::string &filename) {
  return MD5FileSanityCheckWithLimitedSpeed(filename, 0u);
}
bool MD5FileSanityCheckWithLimitedSpeed(const std::string &filename, uint32_t bytes_per_second) {
  std::vector<std::string> field;
  if (!SplitString(filename, "-", &field)) {
    LOG(ERROR) << "MD5FileSanityCheck SpitString fail: " << filename;
    return false;
  }

  std::string md5sum;
  if (!MD5sumFileWithLimitedSpeed(filename, &md5sum, bytes_per_second)) {
    LOG(ERROR) << "MD5sumFile " << filename << " fail";
    return false;
  }
  return md5sum == *field.rbegin();
}

std::string GetDirName(const std::string &path) {
  std::string local_path(path);
  return std::string(dirname(&local_path[0]));
}

std::string GetBaseName(const std::string &path) {
  std::string local_path(path);
  return std::string(basename(&local_path[0]));
}

std::string GetCurrentExecutePath() {
  std::string link = StringPrintf("/proc/self/exe");
  char proc[PATH_MAX] = { 0 };
  int ch = readlink(link.c_str(), proc, arraysize(proc));
  if (ch != -1) {
    return std::string(proc, ch);
  }
  LOG(ERROR) << "GetCurrentExecutePath fail, error: " << errno << " : " << strerror(errno);
  return "";
}

std::string GetCurrentExecuteDir() {
  std::string path = GetCurrentExecutePath();
  return GetDirName(path);
}

int64_t WriteN(int fd, const void * buffer, int64_t size)
{
  if(buffer == NULL || size < 0)
  {
    return -1;
  }
  int64_t writed = 0;
  int64_t res = 0;
  while(writed < size)
  {
    res = write(fd, ((const char*)buffer)+writed, size-writed);
    if(res == 0)
    {
      return writed;
    }
    else if(res == -1)
    {
      if(errno != EAGAIN && errno != EINTR)
      {
        lseek(fd, 0-writed, SEEK_CUR);
        return -1;
      }
    }
    else
    {
      writed += res;
    }
  }
  return writed;
}

int64_t ReadN(int fd, void * buffer, int64_t size)
{
  if(buffer == NULL || size < 0) return -1;
  int64_t bytes_read = 0;
  int64_t res = 0;
  while(bytes_read < size)
  {
    res = read(fd, ((char*)buffer)+bytes_read, size-bytes_read);
    if(res == 0)
    {
      return bytes_read;
    }
    else if(res == -1)
    {
      if(errno == EINTR)
      {
        continue;
      }
      else if(errno == EAGAIN)
      {
        break;
      }
      else
      {
        lseek(fd, 0-bytes_read, SEEK_CUR);
        return -1;
      }
    }
    else
    {
      bytes_read += res;
    }
  }
  return bytes_read;
}

bool CopyFile(const std::string &src_file, const std::string &dst_file)
{
  std::string tmp_path = dst_file + ".XXXXXX";
  if (!MakeUniquePath(&tmp_path[0], 0755)) {
    return false;
  }
  int src_fd = open(src_file.c_str(), O_RDONLY);
  if (src_fd < 0) {
    LOG(ERROR) << "Fail to open file: " << src_file << ": " << strerror(errno);
    return false;
  }
  int dst_fd = open(tmp_path.c_str(), O_CREAT|O_WRONLY, 0755);
  if (dst_fd < 0) {
    close(src_fd);
    LOG(ERROR) << "Fail to open file: " << tmp_path << ": " << strerror(errno);
    return false;
  }
  int64_t buffer_size = 1024*1024;
  std::vector<char> buffer(buffer_size);
  int64_t read_ret = 0;
  int64_t write_ret = 0;
  while (true) {
    read_ret = ReadN(src_fd, &buffer[0], buffer_size);
    if (read_ret < 0) {
      char error_msg[1024];
      LOG(ERROR) << "CopyFile fail, read src_file fail, src_file:" << src_file << " dst_file:" << dst_file 
        << " error_msg:" << strerror_r(errno, error_msg, sizeof(error_msg));
      close(src_fd);
      close(dst_fd);
      remove(tmp_path.c_str());
      return false;
    }
    else if(read_ret == 0) {
      break;
    }
    write_ret = WriteN(dst_fd, &buffer[0], read_ret);
    if (write_ret != read_ret) {
      char error_msg[1024];
      LOG(ERROR) << "CopyFile fail, write dst_file fail, src_file:" << src_file << " dst_file:" << dst_file 
        << " error_msg:" << strerror_r(errno, error_msg, sizeof(error_msg));
      close(src_fd);
      close(dst_fd);
      remove(tmp_path.c_str());
      return false;
    }
  }
  close(src_fd);
  if (close(dst_fd) != 0) {
    char error_msg[1024];
    LOG(ERROR) << "CopyFile fail, close dst_file fail, src_file:" << src_file << " dst_file:" << dst_file 
      << " error_msg:" << strerror_r(errno, error_msg, sizeof(error_msg));
    return false;
  }

  if (rename(tmp_path.c_str(), dst_file.c_str()) != 0) {
    char error_msg[1024];
    LOG(ERROR) << "CopyFile fail, rename fail, src_file:" << src_file << " dst_file:" << dst_file 
      << " error_msg:" << strerror_r(errno, error_msg, sizeof(error_msg));
    remove(tmp_path.c_str());
    return false;
  }
  return true;
}


bool CopyFileToCfs(const std::string &src_file, const std::string &dst_file, int speed_limit_k) {
  int64_t file_size = GetFileSize(src_file);
  if (file_size == -1) {
    return false;
  }
  int32_t write_time_limit = file_size/1024/speed_limit_k+2;
  int pipe_fds[2];
  pipe_fds[0] = pipe_fds[1] = -1;
  if (pipe(pipe_fds) != 0) {
    LOG(ERROR) << "CopyFileToCfs fail, src:" << src_file << " dst:" <<
      dst_file << " pipe fail";
    return false;
  }
  int flags = fcntl(pipe_fds[0], F_GETFL, 0);
  fcntl(pipe_fds[0], F_SETFL, flags|O_NONBLOCK);
  flags = fcntl(pipe_fds[1], F_GETFL, 0);
  fcntl(pipe_fds[1], F_SETFL, flags|O_NONBLOCK);
  pid_t pid = fork();
  if (pid == -1) {
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    LOG(ERROR) << "CopyFileToCfs fail, src:" << src_file << " dst:" <<
      dst_file << " fork fail";
    return false;
  }
  else if (pid == 0) {
    close(pipe_fds[0]);
    signal(SIGPIPE, SIG_IGN);
    FLAGS_minloglevel = 2;
    char notice = 0;
    if (!CopyFile(src_file, dst_file)) {
      notice = 1;
    }
    write(pipe_fds[1], &notice, sizeof(notice));
    close(pipe_fds[1]);
    _exit(0);
  }
  else {
    close(pipe_fds[1]);
    bool ret = false;
    int epoll_fd = -1;
    do {
      epoll_fd = epoll_create(1);
      if (epoll_fd < 0) {
        kill(pid, 9);
        LOG(ERROR) << "CopyFileToCfs fail, src:" << src_file << " dst:" <<
          dst_file << " eopoll_create fail";
        break;
      }
      char notice = 0;
      epoll_event listen_event, return_event;
      listen_event.events = EPOLLIN;
      listen_event.data.fd = pipe_fds[0];
      epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fds[0], &listen_event);
      if (epoll_wait(epoll_fd, &return_event, 1, write_time_limit*1000) == 1) {
        if (read(return_event.data.fd, &notice, sizeof(notice)) == 1 &&
            notice == 0) {
          ret = true;
        } else {
          LOG(ERROR) << "CopyFileToCfs fail, src:" << src_file << " dst:" <<
             dst_file << " child process copy fail";
        }
      } else {
        kill(pid, 9);
        LOG(ERROR) << "CopyFileToCfs fail, src:" << src_file << " dst:" <<
          dst_file << " eopoll_wait fail";
      }
    } while(0);
    if (epoll_fd >= 0) {
      close(epoll_fd);
      epoll_fd = -1;
    }
    close(pipe_fds[0]);
    waitpid(pid, NULL, 0);
    return ret;
  }
}

std::string GetFormatFilename(const std::string& file_pattern,
                              uint32_t seconds_ago) {
  std::string output;
  time_t now;
  time(&now);
  now = now - seconds_ago;
  if (FormatTime(file_pattern, static_cast<double>(now), &output)) {
    return output;
  } else {
    return file_pattern;
  }
}
