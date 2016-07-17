// Copyright (c) 2015, Tencent Inc.
// Author: cernwang <cernwang@tencent.com>

#include "common/file/file_tools.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "app/qzap/common/utility/file_utility.h"
#include "common/base/string/algorithm.h"
#include "common/net/hdfs/hdfs_utility.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"

DEFINE_string(dynamic_dict_tmp_dir, "../tmpdata/", "");

namespace gdt {

static const int kStrNum = 1024;
static const int kCmd = 1024;

bool SortFileByTime(const FileRecord& record_a, const FileRecord& record_b) {
  return record_a.create_time > record_b.create_time;
}

bool FileTools::GetFileTime(const std::string& filename, time_t* time) {
  struct stat statbuf;
  *time = 0;
  if (stat(filename.c_str(), &statbuf) == -1) {
    LOG(ERROR) << "statbuf error\t" << filename;
    return false;
  }
  if (S_ISREG(statbuf.st_mode)) {
    *time = statbuf.st_mtime;
  }
  return true;
}

bool FileTools::FindFileMd5File(
    const std::string& file_dir,
    const std::string& file_prefix,
    std::string* file_md5_name) {
  if (!file_md5_name) {
    return false;
  }
  std::vector<FileRecord> file_list;
  ListAllFile(file_dir, &file_list);
  std::sort(file_list.begin(),
            file_list.end(),
            SortFileByTime);
  file_md5_name->clear();
  for (size_t i = 0; i < file_list.size(); ++i) {
    if (file_list[i].reletive_file_path.find(file_prefix) !=
        std::string::npos) {
      if (CheckMd5(file_list[i].reletive_file_path,
                   file_list[i].absolute_file_path)) {
        LOG(INFO) << "find md5 name\t" << file_list[i].reletive_file_path << "\t"
                  << file_list[i].absolute_file_path;
        *file_md5_name = file_list[i].reletive_file_path;
        return true;
      }
    }
  }
  return false;
}

std::string FileTools::GetFileMd5(const std::string& input_file_path) {
  char cmd[kCmd];
  std::string tmp_md5sum_file =
      FLAGS_dynamic_dict_tmp_dir + "/" + "tmp.md5";
  snprintf(
      cmd, kStrNum * 2,
      "md5sum %s | awk -F\"  \" '{print $1}' > %s",
      input_file_path.c_str(), tmp_md5sum_file.c_str());
  system(cmd);
  std::ifstream fin(tmp_md5sum_file.c_str());
  std::string line_str = "";
  if (!fin.is_open()) {
    LOG(ERROR) << "can't open md5 tmp file\t" << tmp_md5sum_file;
    return line_str;
  }
  if (fin.good() && getline(fin, line_str)) {
    LOG(INFO) << "cmd = " << cmd
              << "file md5 = " << line_str;
    return line_str;
  }
  return line_str;
}

bool FileTools::RemoveFile(
    const std::string& remove_file) {
  char cmd[kCmd];
  snprintf(
      cmd, kStrNum * 2,
      "rm -rf %s", remove_file.c_str());
  system(cmd);
  return true;
}

bool FileTools::Touch(const std::string& file) {
  char cmd[kCmd];
  snprintf(
      cmd, kStrNum * 2,
      "touch %s", file.c_str());
  system(cmd);
  LOG(INFO) << cmd;
  return true;
}

bool FileTools::CopyFile(
    const std::string& source_file,
    const std::string& dest_file) {
  char cmd[kCmd];
  snprintf(
      cmd, kStrNum * 2,
      "cp -rf %s %s", source_file.c_str(), dest_file.c_str());
  system(cmd);
  return true;
}

bool FileTools::RenameFile(
    const std::string& old_name,
    const std::string& new_name) {
  int rename_flag =
      rename(old_name.c_str(), new_name.c_str());
  return rename_flag == 0;
}

bool FileTools::MakeMd5File(
    const std::string& input_file_path,
    std::string* md5_file) {
  if (!md5_file) {
    return false;
  }
  std::string calculate_md5file =
      GetFileMd5(input_file_path);
  *md5_file =
      input_file_path + "_" + calculate_md5file;
  LOG(INFO) << "rename\t" << input_file_path.c_str()
             << "\t" << *md5_file;
  int rename_flag =
      rename(input_file_path.c_str(), md5_file->c_str());
  return rename_flag == 0;
}

bool FileTools::MakeDir(const std::string& dir_path) {
  int status =
      mkdir(dir_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
  if (status != 0) {
    LOG(INFO) << "mkdir failed \t" << dir_path;
    return false;
  }
  LOG(INFO) << "mkdir success \t" << dir_path;
  return true;
}

bool FileTools::ListAllFile(const std::string& dir_path,
    std::vector<FileRecord>* file_list) {
  DIR *current_dir;
  struct dirent *ent;
  char child_path[kStrNum];
  current_dir = opendir(dir_path.c_str());
  if (current_dir == NULL) {
    LOG(ERROR) << "dir not exist dir path = " << dir_path;
    return false;
  }
  memset(child_path, 0, sizeof(child_path));
  while ((ent = readdir(current_dir)) != NULL) {
    snprintf(child_path, kStrNum, "%s/%s", dir_path.c_str(), ent->d_name);
    struct stat info;
    stat(child_path, &info);
    if (S_ISDIR(info.st_mode)) {
      if (strcmp(ent->d_name, ".") == 0 ||
          strcmp(ent->d_name, "..") == 0)
        continue;
      ListAllFile(child_path, file_list);
    } else {
      time_t file_time = 0;
      FileRecord file_record;
      file_record.absolute_file_path =
          dir_path + "/" + std::string(ent->d_name);
      file_record.reletive_file_path =
          std::string(ent->d_name);
      if (GetFileTime(file_record.absolute_file_path, &file_time)) {
        file_record.create_time = file_time;
      }
      file_list->push_back(file_record);
    }
  }
  closedir(current_dir);
  return true;
}

bool FileTools::CheckMd5(
    const std::string file_name, const std::string& file_path) {
  std::string tags_md5sum;
  std::vector<std::string> split_file_vec;
  SplitString(file_name, "_", &split_file_vec);
  if (split_file_vec.empty()) {
    return false;
  }
  tags_md5sum = split_file_vec[split_file_vec.size() - 1];
  std::string calculate_md5sum =
      GetFileMd5(file_path);
  if (tags_md5sum == calculate_md5sum) {
    return true;
  }
  return false;
}

std::string FileTools::GettimeMin() {
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[kStrNum];
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,
           kStrNum,
           "%Y%m%d%H%M",
           timeinfo);
  std::string str(buffer);
  return str;
}

bool FileTools::FindNewFileWithSuccess(
    const std::string& file_dir,
    const std::string& file_prefix,
    std::string* input_new_file) {
  if (!input_new_file) {
    return false;
  }
  std::vector<FileRecord> file_list;
  ListAllFile(file_dir, &file_list);
  std::sort(file_list.begin(),
            file_list.end(),
            SortFileByTime);
  input_new_file->clear();
  for (size_t i = 0; i < file_list.size(); ++i) {
    if (file_list[i].reletive_file_path.find(file_prefix) !=
        std::string::npos) {
      if (!FileExists(file_list[i].absolute_file_path + ".flag")) {
        continue;
      }
      LOG(INFO) << "find md5 name\t" << file_list[i].reletive_file_path << "\t"
                << file_list[i].absolute_file_path;
      if (CheckMd5(file_list[i].reletive_file_path,
                   file_list[i].absolute_file_path)) {
        LOG(INFO) << "find md5 name\t" << file_list[i].reletive_file_path << "\t"
                  << file_list[i].absolute_file_path;
        *input_new_file = file_list[i].reletive_file_path;
        return true;
      }
    }
  }
  return false;
}

bool FileTools::DeTarFile(
    const std::string& source, const std::string& dest) {
  char cmd[kCmd];
  snprintf(
      cmd, kStrNum * 2,
      "tar -xvzf %s -C %s",
      source.c_str(), dest.c_str());
  system(cmd);
  return true;
}

bool FileTools::TarDir(
    const std::string& source_dir, const std::string& dest) {
  char tar_cmd[kCmd];
  snprintf(
      tar_cmd, kStrNum * 2,
      "tar -cvzf %s -C %s/ ./",
      dest.c_str(), source_dir.c_str());
  system(tar_cmd);
  return true;
}

std::string FileTools::GetDate(int32_t day_num = 0) {
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[kStrNum];
  time(&rawtime);
  rawtime += day_num * 24 * 60 * 60;
  timeinfo = localtime(&rawtime);
  strftime(buffer,
           kStrNum,
           "%Y%m%d",
           timeinfo);
  std::string str(buffer);
  return str;
}

std::string FileTools::GetDateByFormat(int32_t day_num = 0) {
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[kStrNum];
  time(&rawtime);
  rawtime += day_num * 24 * 60 * 60;
  timeinfo = localtime(&rawtime);
  strftime(buffer,
           kStrNum,
           "%Y-%m-%d",
           timeinfo);
  std::string str(buffer);
  return str;
}

std::string FileTools::GetHour(int32_t hour_num = 0) {
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[kStrNum];
  time(&rawtime);
  rawtime += hour_num * 60 * 60;
  timeinfo = localtime(&rawtime);
  strftime(buffer,
           kStrNum,
           "%Y%m%d%H",
           timeinfo);
  std::string str(buffer);
  return str;
}

bool FileTools::FileExists(const std::string& file_path) {
  struct stat file_info;
  return stat(file_path.c_str(), &file_info) == 0;
}

bool FileTools::CopyHdfsTextFileToLocal(
    const std::string& hadoop_dir, const std::string& tdw_pass_word,
    const std::string& local_file, const std::string& dfs_name = "",
    bool need_date = false, bool need_password = false,
    bool use_dfs_name = false) {
  if (!::gdt::hdfs::IsHDFSFile(hadoop_dir)) {
    LOG(ERROR) << "Not Hdfs File tdw path\t" << hadoop_dir;
    return false;
  }
  std::string hadoop_cmd = "hadoop fs ";
  if (use_dfs_name) {
    hadoop_cmd += " -Dfs.default.name=" + dfs_name;
  }
  if (need_password) {
    hadoop_cmd += " -Dhadoop.job.ugi=" + tdw_pass_word;
  }
  hadoop_cmd += " -text " + hadoop_dir;
  if (need_date) {
    hadoop_cmd += FileTools::GetDate(-2);
  }
  hadoop_cmd += "/* > " + local_file;
  // 更多的检查 to_do
  LOG(ERROR) << hadoop_cmd;
  system(hadoop_cmd.c_str());
  return true;
}

bool FileTools::CompareWithFile(
    const std::string& source_file,
    const std::string& dest_file) {
  std::string md5sum_source;
  if (!MD5sumFile(source_file, &md5sum_source)) {
    LOG(ERROR) << "MD5sumFile : " << md5sum_source << " fail";
    return false;
  }
  std::string md5sum_dest;
  if (!MD5sumFile(dest_file, &md5sum_dest)) {
    LOG(ERROR) << "MD5sumFile : " << dest_file << " fail";
    return false;
  }
  return md5sum_source == md5sum_dest;
}

}  // namespace gdt
