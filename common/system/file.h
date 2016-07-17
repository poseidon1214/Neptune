// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: Liu Xiaokang <hsiaokangliu@tencent.com>
// Modified By: Pei Xingxin <michaelpei@tencent.com>

#ifndef COMMON_SYSTEM_FILE_H_
#define COMMON_SYSTEM_FILE_H_

/// @brief file operation interfaces.
/// @author hsiaokangliu

#include <stdint.h>
#include <string>
#include <vector>

namespace gdt {

namespace file {

bool Copy(const char* src, const char* dest, bool overwrite = false);
bool Copy(const std::string& src, const std::string& dest,
    bool overwrite = false);
bool Delete(const char* filename);
bool Delete(const std::string& filename);
bool Rename(const char* oldname, const char* newname);
bool Rename(const std::string& oldname, const std::string& newname);

bool IsRegular(const char* filename);
bool IsRegular(const std::string& filename);
bool Exists(const char* filename);
bool Exists(const std::string& filename);

bool IsReadable(const char* filename);
bool IsReadable(const std::string& filename);
bool IsWritable(const char* filename);
bool IsWritable(const std::string& filename);

bool GetSize(const char* filename, uint64_t* size);
bool GetSize(const std::string& filename, uint64_t* size);
int64_t GetSize(const char* filename);
int64_t GetSize(const std::string& filename);

struct FileTime {
  int64_t create_time_;
  int64_t access_time_;
  int64_t modify_time_;
};

bool GetTime(const std::string& filename, FileTime* times);
bool SetTime(const std::string& filename, const FileTime& times);

bool Touch(const std::string& filename);

time_t GetAccessTime(const char* filename);
time_t GetAccessTime(const std::string& filename);
time_t GetCreateTime(const char* filename);
time_t GetCreateTime(const std::string& filename);
time_t GetLastModifyTime(const char* filename);
time_t GetLastModifyTime(const std::string& filename);

/// real all content into container
bool ReadAll(const std::string& filename, void* buffer,
    size_t buffer_size, size_t* read_size);
bool ReadAll(const std::string& filename, std::string* content);
bool ReadAll(const std::string& filename, std::vector<char>* content);

bool WriteTo(const std::string& filename, const std::string& content);
bool WriteTo(const std::string& filename, const std::vector<char>& content);
bool WriteTo(const std::string& filename, const void* content, size_t size);
bool AppendTo(const std::string& filename, const std::string& content);
bool AppendTo(const std::string& filename, const std::vector<char>& content);
bool AppendTo(const std::string& filename, const void* content, size_t size);

}  // namespace file

}  // namespace gdt

#endif  // COMMON_SYSTEM_FILE_H_
