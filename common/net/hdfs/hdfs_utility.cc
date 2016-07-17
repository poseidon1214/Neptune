//====================================================================
//
// Copyright (C) ,  2015, Tencent Inc. All righs Reserved.
//
// Filename:      sync_file.cc
// Author:        XingxinPei(michaelpei) <michaelpei@tencent.com>
// Description:   HDFS operation
//
// $Id:$
//
//====================================================================
#include "common/net/hdfs/hdfs_utility.h"

DEFINE_string(hadoop_location, "",
              "which hadoop in case can not find hadoop");

namespace gdt {
namespace hdfs {

/* use pipe send "ls" to hdfs system */
static const std::vector<std::string> GetFileHandle(
    const std::string& output) {
  std::string delim = " ";
  std::vector<std::string> tmp_result;
  SplitString(output, delim.c_str(), &tmp_result);
  std::vector<std::string> result;
  for (size_t i = 0; i < tmp_result.size(); i++) {
    if (IsHDFSFile(tmp_result[i])) {
      std::string second_delim = "\n";
      std::vector<std::string> tmp;
      SplitString(tmp_result[i], second_delim.c_str(), &tmp);
      if (!tmp.empty()) {
        result.push_back(tmp[0]);
      }
    }
  }
  return result;
}

/* to abstract the process visit hdfs system with "cmd" method.  
 * param2 could be an empty string, like "-ls"
 * result could be an empty string, like "copyToLocal"
 * handle function pointer could be null, if no interest in processing result 
 */
static int RunCmd(
    const std::string& cmd, const std::string& param1,
    const std::string& param2, std::vector<std::string>* result,
    const std::vector<std::string> (*handle)(const std::string& ret_string)) {
  std::string cp_cmd = FLAGS_hadoop_location + "hadoop fs ";
  cp_cmd += " -" + cmd + " ";
  cp_cmd += param1;
  cp_cmd += " ";
  cp_cmd += param2;
  cp_cmd += " 2>&1 ";
  VLOG(500) << "Execute cmd " << cp_cmd;

  /* open shell process to execute the cmd */
  errno = 0;
  FILE* fp = popen(cp_cmd.c_str(), "r");
  if (!fp) {
    LOG(ERROR) << "Execute cmd " << cp_cmd << " with error" << strerror(errno);
    return errno;
  }

  char buffer[256];
  std::string output;
  while (fgets(buffer, sizeof(buffer), fp)) {
    output += buffer;
  }

  int status = pclose(fp);
  if (status == -1 || WEXITSTATUS(status)) {
    LOG(ERROR) << "Execute cmd " << cp_cmd << " failed, exit code: "
      << ((status == -1) ? status : WEXITSTATUS(status))
      << ", output: " << output;
    return (status == -1) ? status : WEXITSTATUS(status);
  }

  /* process the output, and get the target result */
  if (handle) {
    *result = handle(output);
  }
  return 0;
}

int GetRemoteFileList(const std::string& dir_path,
                      std::vector<std::string>* result) {
  if (IsHDFSFile(dir_path)) {
    std::string cmd = "ls";
    std::string param2 = "";
    int ret = RunCmd(cmd, dir_path, param2, result, GetFileHandle);
    return ret;
  }
  return -1;
}

static int CopyHDFSToLocal(const std::string& src, const std::string& dst) {
  std::string cmd = "copyToLocal";
  std::vector<std::string> result;
  int ret = RunCmd(cmd, src, dst, &result, NULL);
  return ret;
}

int CopyRemoteToLocal(const std::string& remote_path,
                      const std::string& local_path) {
  if (IsHDFSFile(remote_path)) {
    return CopyHDFSToLocal(remote_path, local_path);
  }
  return -1;
}

int CopyRemoteToLocalWithoutCheckHead(const std::string& remote_path,
                      const std::string& local_path) {
  return CopyHDFSToLocal(remote_path, local_path);
}

bool IsHDFSFile(const std::string& file_path) {
  return file_path.size() >= sizeof(HDFS_PREFIX) - 1
    && file_path.compare(0, sizeof(HDFS_PREFIX) - 1, HDFS_PREFIX) == 0;
}

}  // namespace hdfs
}  // namespace gdt
