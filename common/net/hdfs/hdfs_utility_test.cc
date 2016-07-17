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

#include "thirdparty/gtest/gtest.h"

namespace gdt {
namespace hdfs {

TEST(IsHDFSFile, Test) {
  EXPECT_TRUE(IsHDFSFile("hdfs://****"));
  EXPECT_TRUE(IsHDFSFile("hdfs://"));
  EXPECT_FALSE(IsHDFSFile("hdfs:/"));
  EXPECT_FALSE(IsHDFSFile("hdfs:"));
  EXPECT_FALSE(IsHDFSFile("hdfs*:"));
  EXPECT_FALSE(IsHDFSFile("hdfs*//"));
}

TEST(CopyRemoteToLocal, FailHDFSPath) {
  std::string local_path = "./";
  EXPECT_EQ(-1, CopyRemoteToLocal("hdfs", local_path));
  EXPECT_EQ(-1, CopyRemoteToLocal("hdfs:/", local_path));
  EXPECT_NE(0, CopyRemoteToLocal("hdfs://a", local_path));
}

TEST(GetRemoteFileList, FailHDFSPath) {
  std::vector<std::string> result;
  EXPECT_EQ(-1, GetRemoteFileList("hdfs", &result));
  EXPECT_EQ(-1, GetRemoteFileList("hdfs:/", &result));
  EXPECT_NE(0, GetRemoteFileList("hdfs://a", &result));
}

}  // namespace hdfs
}  // namespace gdt
