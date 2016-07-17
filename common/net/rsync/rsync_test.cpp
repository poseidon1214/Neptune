// Copyright (c) 2011, Tencent Inc.  All rights reserved.
// Author: Wenting Liu <wentingliu@tencent.com>
// Created: 11/11/15
// Description: common rsync test class

#include "common/net/rsync/rsync.h"

#include <string>

#include "app/qzap/common/utility/file_utility.h"
#include "app/qzap/common/base/string_utility.h"
#include "common/base/string/concat.h"
#include "thirdparty/gtest/gtest.h"


TEST(Rsync, Pull) {
  gdt::Rsync::Options options;
  options.SetAppend(true);
  gdt::Rsync rsync(options);

  char tmp_file[] = "rsync_test_file";
  unlink(tmp_file);
  std::string rs = rsync.Pull("", "/etc/passwd", tmp_file);
  EXPECT_EQ(rs, "");
  EXPECT_TRUE(FileExisting(tmp_file));
  unlink(tmp_file);

  char tmp_folder[] = "rsync_test_folder/";
  system(StringConcat("rm -rf ", tmp_folder).c_str());
  rs = rsync.Pull("", "/etc/passwd", tmp_folder);
  EXPECT_EQ(rs, "");
  EXPECT_TRUE(FileExisting(tmp_folder));
  system(StringConcat("rm -rf ", tmp_folder).c_str());
}
