// Copyright (c) 2015, Tencent Inc.
// Author: cernwang<cernwang@tencent.com>
// 这些代码以后要检查返回值

#ifndef COMMON_FILE_FILE_TOOLS_H_
#define COMMON_FILE_FILE_TOOLS_H_

#include <stdint.h>
#include <time.h>
#include <vector>
#include <string>

namespace gdt {

struct FileRecord {
  std::string absolute_file_path;
  std::string reletive_file_path;
  time_t create_time;
};

class FileTools {
 public:
  static std::string GenFilePathWithData(
      const std::string& file_dir,
      const std::string& file_path,
      const std::string& data) {
    std::string new_file
        = file_dir + "/" + data + "/" + file_path;
    return new_file;
  }

  static bool GetFileTime(const std::string& filename, time_t* time);

  static bool CopyFile(const std::string& source_file,
                       const std::string& dest_file);

  static bool ListAllFile(const std::string& dir_path,
      std::vector<FileRecord>* file_list);

  static bool CheckMd5(
      const std::string file_name, const std::string& file_path);

  static bool DeTarFile(
      const std::string& source,
      const std::string& dest);

  static bool TarFile(
      const std::string& source,
      const std::string& dest);

  // -1 yesterday; 1 tomorrow
  static std::string GetDate(int32_t day_num);

  // -1 yesterday; 1 tomorrow
  static std::string GetDateByFormat(int32_t day_num);

  // -1 last hour; 1 next hour
  static std::string GetHour(int32_t day_num);

  static bool FileExists(const std::string& file_path);

  // 寻找file_dir下满足file_prefix，且包含.success的文件
  static bool FindNewFileWithSuccess(
      const std::string& file_dir,
      const std::string& file_prefix,
      std::string* input_new_file);

  static bool CompareWithFile(
      const std::string& source_file,
      const std::string& dest_file);

  // 寻找file_dir下满足file_prefix，且md5计算正确的文件
  // md5在文件中获取
  static bool FindFileMd5File(
      const std::string& file_dir,
      const std::string& file_prefix,
      std::string* file_md5_name);

  static bool RemoveFile(const std::string& remove_file);

  static std::string GetFileMd5(
      const std::string& input_file_path);

  static bool MakeMd5File(
      const std::string& input_file_path,
      std::string* md5_file);

  static bool TarDir(
      const std::string& source_dir,
      const std::string& dest);

  static bool Touch(
      const std::string& file);

  static bool RenameFile(
      const std::string& old_name,
      const std::string& new_name);

  static std::string GettimeMin();

  static bool MakeDir(const std::string& dir_path);

  // password_example: tdw_cernwang:cernwang,g_sng_gdt_isd_gdt
  static bool CopyHdfsTextFileToLocal(
      const std::string& hadoop_dir, const std::string& tdw_pass_word,
      const std::string& local_file, const std::string& dfs_name,
      bool need_date, bool need_password, bool use_dfs_name);
};

}  // namespace gdt
#endif  // COMMON_FILE_FILE_TOOLS_H_
