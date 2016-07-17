// Copyright (c) 2012, Tencent Inc. All rights reserved.
// Author: Liu Xiaokang <hsiaokangliu@tencent.com>

#include "common/system/directory.h"

#include <string>
#include <vector>

#include "common/system/file.h"
#include "common/system/path.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

TEST(DirectoryTest, Ops) {
  std::string current_dir = gdt::directory::GetCurrentDir();
  ASSERT_EQ(gdt::directory::ToUnixFormat(
        gdt::directory::ToWindowsFormat(current_dir)), current_dir);

  std::string tmp_dir = "_test_dir_";
  ASSERT_FALSE(gdt::directory::Exists(tmp_dir));
  gdt::directory::Create(tmp_dir);
  ASSERT_TRUE(gdt::directory::Exists(tmp_dir));
  ASSERT_TRUE(gdt::directory::IsReadable(tmp_dir));
  ASSERT_TRUE(gdt::directory::IsWritable(tmp_dir));

  std::string bad_dir = "/non-existed-dir";
  ASSERT_FALSE(gdt::directory::IsReadable(bad_dir));
  ASSERT_FALSE(gdt::directory::IsWritable(bad_dir));

  std::string new_current = current_dir + "/" + tmp_dir;
  ASSERT_FALSE(gdt::directory::SetCurrentDir(bad_dir));
  gdt::directory::SetCurrentDir(new_current);
  ASSERT_EQ(gdt::directory::GetCurrentDir(), new_current);
  gdt::directory::SetCurrentDir("..");
  ASSERT_EQ(gdt::directory::GetCurrentDir(), current_dir);

  ASSERT_TRUE(gdt::directory::Delete(tmp_dir));
  ASSERT_FALSE(gdt::directory::Exists(tmp_dir));

  std::vector<std::string> files;
  LOG(INFO) << "Files under current dir:\n*****************";
  gdt::directory::GetFiles(current_dir, &files);
  for (size_t i = 0; i < files.size(); i++) {
    LOG(INFO) <<  files[i];
  }
  LOG(INFO) << "Dirs under current dir:\n*****************";
  gdt::directory::GetSubDirs(current_dir, &files);
  for (size_t i = 0; i < files.size(); i++) {
    LOG(INFO) << files[i];
  }
  LOG(INFO) << "All files under current dir:\n*****************";
  gdt::directory::GetAllFiles(current_dir, &files);
  for (size_t i = 0; i < files.size(); i++) {
    LOG(INFO) <<  files[i];
  }
  LOG(INFO) << "All dirs under current dir:\n*****************";
  gdt::directory::GetAllSubDirs(current_dir, &files);
  for (size_t i = 0; i < files.size(); i++) {
    LOG(INFO) << files[i];
  }
}

TEST(DirectoryIteratorTest, Ops) {
  std::string str = ".";
  gdt::DirectoryIterator iter;

  if (!iter.Open(str)) {
    LOG(INFO) << "open failed.";
    return;
  }

  LOG(INFO) << "All dir and files:\n==============";
  while (!iter.IsEnd()) {
    LOG(INFO) << iter.Name() << std::endl;
    LOG(INFO) << "FullName: " << iter.FullPath() << std::endl;
    iter.Next();
  }
  iter.Close();

  LOG(INFO) << "All files:\n==============";
  if (!iter.Open(str, gdt::DirectoryIterator::FILE)) {
    LOG(INFO) << "open failed.";
    return;
  }
  while (!iter.IsEnd()) {
    LOG(INFO) << iter.Name() << std::endl;
    LOG(INFO) << "FullName: " << iter.FullPath();
    iter.Next();
  }
  iter.Close();
  LOG(INFO) << "All cpps:\n==============";

  if (!iter.Open(str, gdt::DirectoryIterator::FILE, "*.cpp")) {
    LOG(INFO) << "open failed.";
    return;
  }
  while (!iter.IsEnd()) {
    LOG(INFO) << iter.Name() << std::endl;
    LOG(INFO) << "FullName: " << iter.FullPath() << std::endl;
    iter.Next();
  }
  iter.Close();
}

TEST(DirectoryIteratorTest, FilterIterator) {
  std::string str = ".";
  gdt::DirectoryIterator iter(str.c_str(),
      gdt::DirectoryIterator::FILE, "*.cpp");
  while (!iter.IsEnd()) {
    LOG(INFO) << iter.Name() << std::endl;
    LOG(INFO) << "FullName: " << iter.FullPath() << std::endl;
    iter.Next();
  }
  iter.Close();
}

TEST(DirectoryTest, Time) {
  std::string tmp_dir = "_test_dir_";
  ASSERT_FALSE(gdt::directory::Exists(tmp_dir));
  gdt::directory::Create(tmp_dir);
  ASSERT_TRUE(gdt::directory::Exists(tmp_dir));

  time_t now = time(NULL);
  time_t create_time = gdt::directory::GetCreateTime(tmp_dir);
  time_t access_time = gdt::directory::GetAccessTime(tmp_dir);
  time_t modify_time = gdt::directory::GetLastModifyTime(tmp_dir);
  EXPECT_LT(llabs(create_time - now), 5);
  EXPECT_LT(llabs(access_time - now), 5);
  EXPECT_LT(llabs(modify_time - now), 5);

  gdt::directory::Delete(tmp_dir);
  ASSERT_FALSE(gdt::directory::Exists(tmp_dir));

  ASSERT_EQ(-1, gdt::directory::GetCreateTime("/bin/ls"));
  ASSERT_EQ(-1, gdt::directory::GetAccessTime("/bin/ls"));
  ASSERT_EQ(-1, gdt::directory::GetLastModifyTime("/bin/ls"));
}

TEST(DirectoryTest, RecursiveDelete) {
  std::string tmp_dir = "_test_dir_";
  ASSERT_FALSE(gdt::directory::Exists(tmp_dir));
  gdt::directory::Create(tmp_dir);
  ASSERT_TRUE(gdt::directory::Exists(tmp_dir));
  std::string sub_dir =  tmp_dir + "/test1";
  gdt::directory::Create(sub_dir);
  sub_dir =  tmp_dir + "/test2";
  gdt::directory::Create(sub_dir);
  ASSERT_TRUE(gdt::directory::RecursiveDelete(tmp_dir));
}
