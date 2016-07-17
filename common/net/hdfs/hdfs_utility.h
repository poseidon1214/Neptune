//====================================================================
//
// Copyright (C) ,  2015, Tencent Inc. All righs Reserved.
//
// Filename:      sync_file.cc
// Author:        XingxinPei(michaelpei) <michaelpei@tencent.com>
// Description:   HDFS operation
//                In some environment, the program can not find the "hadoop"
//                command, so We provide GFLAGS "hadoop_location" to
//                help define the envrionment. For example, "which hadoop"
//                lists /usr/local/bin/hadoop, in your gflags, set
//                --hadoop_location=/usr/local/bin/
// $Id:$
//
//====================================================================
#ifndef COMMON_NET_HDFS_HDFS_UTILITY_H_
#define COMMON_NET_HDFS_HDFS_UTILITY_H_
#pragma once

#include <string>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "common/base/string/algorithm.h"

namespace gdt {
namespace hdfs {

static const char* const HDFS_PREFIX = "hdfs://";

bool IsHDFSFile(const std::string& file_path);

int CopyRemoteToLocal(const std::string& remote_path,
                      const std::string& local_path);

int GetRemoteFileList(const std::string& dir_path,
                      std::vector<std::string>* result);

int CopyRemoteToLocalWithoutCheckHead(const std::string& remote_path,
                      const std::string& local_path);

}  // namespace hdfs
}  // namespace gdt
#endif  // COMMON_NET_HDFS_HDFS_UTILITY_H_
