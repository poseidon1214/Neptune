// Copyright (c) 2011, Tencent Inc. All rights reserved.
// Author: Liu Xiaokang <hsiaokangliu@tencent.com>
// Modified By: Pei Xingxin <michaelpei@tencent.com>

#include "common/system/file.h"

#include <stdint.h>
#include <string>
#include <vector>

#include "thirdparty/gtest/gtest.h"

TEST(FileTest, Ops) {
  std::string original_file = "/bin/ls";
  std::string current_file = "/tmp/ls";
  gdt::file::Copy(original_file, current_file);
  ASSERT_TRUE(gdt::file::IsRegular(current_file));
  std::string bak_file = "/tmp/ls.bak";
  gdt::file::Copy(current_file, bak_file);
  ASSERT_TRUE(gdt::file::IsRegular(bak_file));
  gdt::file::Delete(bak_file);
  ASSERT_FALSE(gdt::file::IsRegular(bak_file));

  EXPECT_TRUE(gdt::file::Touch(bak_file));
  EXPECT_TRUE(gdt::file::Copy(current_file, bak_file, true));
  uint64_t size1, size2;
  EXPECT_TRUE(gdt::file::GetSize(current_file, &size1));
  EXPECT_TRUE(gdt::file::GetSize(bak_file, &size2));
  ASSERT_EQ(size1, size2);
  gdt::file::Delete(bak_file);
  ASSERT_FALSE(gdt::file::IsRegular(bak_file));

  gdt::file::Rename(current_file, bak_file);
  ASSERT_TRUE(gdt::file::Exists(bak_file));
  ASSERT_FALSE(gdt::file::Exists(current_file));

  gdt::file::Rename(bak_file, current_file);
  ASSERT_TRUE(gdt::file::IsReadable(current_file));
  ASSERT_TRUE(gdt::file::IsWritable(current_file));
}

TEST(FileTest, Time) {
  std::string original_file = "/bin/ls";
  std::string current_file = "/tmp/ls";
  gdt::file::Delete(current_file);
  gdt::file::Copy(original_file, current_file);
  time_t now = time(NULL);
  gdt::file::FileTime ft;
  ASSERT_TRUE(gdt::file::GetTime(current_file, &ft));
  EXPECT_LT(llabs(ft.create_time_ - now), 5);
  EXPECT_LT(llabs(ft.access_time_ - now), 5);
  EXPECT_LT(llabs(ft.modify_time_ - now), 5);

  sleep(2);
  gdt::file::Touch(current_file);
  now = time(NULL);
  ASSERT_TRUE(gdt::file::GetTime(current_file, &ft));
  EXPECT_LT(llabs(ft.create_time_ - now), 5);
  EXPECT_LT(llabs(ft.access_time_ - now), 5);
  EXPECT_LT(llabs(ft.modify_time_ - now), 5);

  ft.create_time_ = ft.create_time_ - 100;
  ft.access_time_ = ft.access_time_ - 100;
  ft.modify_time_ = ft.modify_time_ - 100;
  ASSERT_TRUE(gdt::file::SetTime(current_file, ft));
  ASSERT_EQ(ft.access_time_, gdt::file::GetAccessTime(current_file));
  // Create time will not change.
  ASSERT_EQ(ft.create_time_ + 100, gdt::file::GetCreateTime(current_file));
  ASSERT_EQ(ft.modify_time_, gdt::file::GetLastModifyTime(current_file));

  gdt::file::Delete(current_file);
}

TEST(FileTest, ReadFile) {
  std::string str_content;
  ASSERT_TRUE(gdt::file::ReadAll("test_unix.txt", &str_content));
  std::vector<char> vec_content;
  ASSERT_TRUE(gdt::file::ReadAll("test_unix.txt", &vec_content));
  size_t buffer_size = 1024 * 8;
  char* buffer = new char[buffer_size];
  size_t read_size = 0;
  ASSERT_TRUE(gdt::file::ReadAll("test_unix.txt", buffer,
        buffer_size, &read_size));
  ASSERT_EQ(str_content.size(), vec_content.size());
  ASSERT_EQ(str_content.size(), read_size);
  ASSERT_STREQ(str_content.c_str(), buffer);
  ASSERT_STREQ(str_content.c_str(), &vec_content[0]);
  delete []buffer;
}

TEST(FileTest, WriteFile) {
  std::string content("data");
  std::string buffer;
  ASSERT_TRUE(gdt::file::WriteTo("tmp.txt", content));
  ASSERT_TRUE(gdt::file::ReadAll("tmp.txt", &buffer));
  ASSERT_STREQ(buffer.c_str(), "data");
  ASSERT_TRUE(gdt::file::WriteTo("tmp.txt", content));
  ASSERT_TRUE(gdt::file::ReadAll("tmp.txt", &buffer));
  ASSERT_STREQ(buffer.c_str(), "data");

  ASSERT_TRUE(gdt::file::AppendTo("tmp.txt", content));
  ASSERT_TRUE(gdt::file::ReadAll("tmp.txt", &buffer));
  ASSERT_STREQ(buffer.c_str(), "datadata");
  ASSERT_TRUE(gdt::file::Delete("tmp.txt"));
  ASSERT_FALSE(gdt::file::AppendTo("tmp.txt", content));

  std::vector<char> c1(3, '1');
  ASSERT_TRUE(gdt::file::WriteTo("tmp.txt", c1));
  ASSERT_TRUE(gdt::file::AppendTo("tmp.txt", c1));
  ASSERT_TRUE(gdt::file::ReadAll("tmp.txt", &buffer));
  ASSERT_STREQ(buffer.c_str(), "111111");

  char c2[] = {'2', '2', '2'};
  ASSERT_TRUE(gdt::file::WriteTo("tmp.txt", c2, 3));
  ASSERT_TRUE(gdt::file::AppendTo("tmp.txt", c2, 3));
  ASSERT_TRUE(gdt::file::ReadAll("tmp.txt", &buffer));
  ASSERT_STREQ(buffer.c_str(), "222222");
}
