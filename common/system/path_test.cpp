// Copyright (c) 2012, Tencent Inc. All rights reserved.
// Author: Liu Xiaokang <hsiaokangliu@tencent.com>

#include "common/system/path.h"
#include <string>
#include <vector>
#include "thirdparty/gtest/gtest.h"

namespace gdt {

TEST(Path, Ops) {
  std::string filepath = "io_test";
  std::string fullpath = Path::GetCurrentDir() + "/" + filepath;
  EXPECT_EQ(fullpath, Path::ToAbsolute(filepath));

  EXPECT_EQ(filepath, Path::GetBaseName(filepath));
  EXPECT_EQ("", Path::GetExtension(filepath));
  EXPECT_EQ("", Path::GetDirectory(filepath));

  filepath = "/";
  EXPECT_EQ("", Path::GetBaseName(filepath));
  EXPECT_EQ("", Path::GetExtension(filepath));
  EXPECT_EQ("/", Path::GetDirectory(filepath));

  filepath = "////xxx//xx.x";
  EXPECT_EQ("xx.x", Path::GetBaseName(filepath));
  EXPECT_EQ(".x", Path::GetExtension(filepath));
  EXPECT_EQ("////xxx//", Path::GetDirectory(filepath));
}

TEST(Path, Normalize) {
  EXPECT_EQ(".", Path::Normalize(""));
  EXPECT_EQ("/", Path::Normalize("///"));
  EXPECT_EQ("//", Path::Normalize("//"));
  EXPECT_EQ("//abc", Path::Normalize("//abc"));
  EXPECT_EQ("/a/b/c", Path::Normalize("///a//b/c//"));
  EXPECT_EQ("../..", Path::Normalize("../../"));
  EXPECT_EQ("../../abc", Path::Normalize("../../abc"));
  EXPECT_EQ("/abc", Path::Normalize("/data/../abc"));
  EXPECT_EQ("/", Path::Normalize("/abc/../../../"));
}

TEST(Path, Join) {
  EXPECT_EQ("a/b", Path::Join("a", "b"));
  EXPECT_EQ("a/b/c", Path::Join("a", "b", "c"));
  EXPECT_EQ("a/b/c/d", Path::Join("a", "b", "c", "d"));
  EXPECT_EQ("a/b/c/d/e", Path::Join("a", "b", "c", "d", "e"));
  EXPECT_EQ("a/b/c/d/e/f", Path::Join("a", "b/", "c", "d/", "e", "f"));
  EXPECT_EQ("abc/def/", Path::Join("abc", "def/"));
  EXPECT_EQ("/abc/def/", Path::Join("/abc", "def/"));
  EXPECT_EQ("/abc/def/", Path::Join("/abc/", "def/"));
  EXPECT_EQ("/def", Path::Join("/abc/", "/def"));
}

TEST(Path, IsAbsolute) {
  EXPECT_TRUE(Path::IsAbsolute("/"));
  EXPECT_TRUE(Path::IsAbsolute("/usr"));
  EXPECT_FALSE(Path::IsAbsolute("home"));
}

TEST(Path, ToAbsolute) {
  std::string path = "/xfs/szsk-processing/home/hotwheels/taskmanager";
  EXPECT_EQ(path, Path::ToAbsolute(path));
  EXPECT_EQ("/xfs/d", Path::ToAbsolute("/xfs/abc/../d"));
  EXPECT_EQ(Path::GetCurrentDir() + "/d", Path::ToAbsolute("abc/../d"));
}

TEST(Path, Exist) {
  EXPECT_TRUE(Path::Exists("/"));
  EXPECT_TRUE(Path::Exists("/bin"));
  EXPECT_TRUE(Path::Exists("/bin/sh"));
  EXPECT_FALSE(Path::Exists("/non-exist"));
}

TEST(Path, Type) {
  EXPECT_TRUE(Path::IsDir("/"));
  EXPECT_FALSE(Path::IsFile("/"));
  EXPECT_TRUE(Path::IsDir("/bin"));
  EXPECT_FALSE(Path::IsFile("/bin"));
  EXPECT_FALSE(Path::IsDir("/bin/sh"));
  EXPECT_TRUE(Path::IsFile("/bin/sh"));
  EXPECT_FALSE(Path::IsDir("/non-exist"));
  EXPECT_FALSE(Path::IsFile("/non-exist"));
}

static void TouchFile(const char* path) {
  FILE* fp = fopen(path, "w");
  fclose(fp);
}

TEST(Path, Time) {
  time_t now = time(NULL);
  const char* path = "time_test";
  TouchFile(path);
  time_t change_time = Path::GetChangeTime(path);
  time_t access_time = Path::GetAccessTime(path);
  time_t modify_time = Path::GetModifyTime(path);
  EXPECT_LT(llabs(change_time - now), 5);
  EXPECT_LT(llabs(access_time - now), 5);
  EXPECT_LT(llabs(modify_time - now), 5);
  remove(path);
}

}  // namespace gdt
