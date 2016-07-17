// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-2-26
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <string>
#include <vector>
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/walltime.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/file_utility.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/zlib/zlib.h"

class FileUtilityTest : public testing::Test {
 public:
  void SetUp() {
    char test_tmp_path[] = "/tmp/file_utility_test_XXXXXX";
    ASSERT_TRUE(MakeUniquePath(test_tmp_path, 0755));
    test_tmp_path_ = test_tmp_path;
    std::string tmp_file = test_tmp_path_ + "/a.XXXXXX";
    ASSERT_TRUE(MakeUniquePath(&tmp_file[0], 0755));
  }

  void TearDown() {
    system("rm -rf /tmp/file_utility_test*");
  }

  void CreateTempFile(const std::string &content, std::string *filename) {
    char local_filename[] = "/tmp/file_utility_test_rand.XXXXXX";
    int fd = mkstemp(local_filename);
    CHECK_NE(fd, -1);
    close(fd);
    CHECK(WriteStringToFile(local_filename, content));
    filename->assign(local_filename);
  }

  void CreateLastModifyTempFile(const std::string &content, std::string *filename) {
    char local_filename[] = "/tmp/last_modify_file_test_XXXXXX";
    int fd = mkstemp(local_filename);
    CHECK_NE(fd, -1);
    close(fd);
    CHECK(WriteStringToFile(local_filename, content));
    filename->assign(local_filename);
  }
 protected:
  std::string test_tmp_path_;
};

TEST_F(FileUtilityTest, TestIsDir) {
  VLOG(2) << test_tmp_path_;
  ASSERT_TRUE(IsDir(test_tmp_path_));
}

TEST_F(FileUtilityTest, TestReadFileToString) {
  std::string filename;
  std::string s1("helloworld");
  std::string s2;
  CreateTempFile(s1, &filename);
  ASSERT_TRUE(ReadFileToString(filename, &s2));
  ASSERT_EQ(s1, s2);
  unlink(filename.c_str());

  int i;
  for (i = 0; i < 1024; ++i) {
    s1 += "world";
  }
  CreateTempFile(s1, &filename);
  uint32_t offset = 10;
  int32_t length = 2148;
  s2.clear();
  ASSERT_TRUE(ReadFileOffsetToString(filename,
              offset, length, &s2));
  ASSERT_EQ(s1.substr(offset, length), s2);
  unlink(filename.c_str());
}

TEST_F(FileUtilityTest, TestReadFileToStringWithLimitedSpeed) {
  std::string filename;
  std::string s1("helloworld");
  std::string s2;
  CreateTempFile(s1, &filename);
  ASSERT_TRUE(ReadFileToStringWithLimitedSpeed(filename, &s2, 10u));
  ASSERT_EQ(s1, s2);
  unlink(filename.c_str());

  s1.resize(1024 * 1024 * 5);
  CreateTempFile(s1, &filename);
  uint32_t offset = 10;
  int32_t length = s1.size() - 10;
  s2.clear();
  double begin = WallTime_Now();
  ASSERT_TRUE(ReadFileOffsetToString(filename,
              offset, length, &s2));
  double end = WallTime_Now();
  LOG(ERROR) << " filename " << filename << " filesize " << GetFileSize(filename)
    << "string size " << s1.size();
  LOG(ERROR) << "read time with unlimited speed :" << end - begin;
  s2.clear();
  double begin_limited = WallTime_Now();
  ASSERT_TRUE(ReadFileOffsetToStringWithLimitedSpeed(filename,
              offset, length, &s2, 20u << 20));
  double end_limited = WallTime_Now();
  LOG(ERROR) << "read time with limited speed :" << end_limited - begin_limited;
  EXPECT_GE((end_limited - begin_limited) * (20u << 20) * 1.1 + 4096, length);
  ASSERT_EQ(s1.substr(offset, length), s2);
  unlink(filename.c_str());
}
TEST_F(FileUtilityTest, TestReadFileToStringWithLimitedSpeed100M) {
  std::string filename;
  std::string s1("helloworld");
  std::string s2;
  CreateTempFile(s1, &filename);
  ASSERT_TRUE(ReadFileToStringWithLimitedSpeed(filename, &s2, 10u));
  ASSERT_EQ(s1, s2);
  unlink(filename.c_str());

  s1.resize(100ull << 20);
  CreateTempFile(s1, &filename);
  s2.clear();
  double begin_limited = WallTime_Now();
  ASSERT_TRUE(ReadFileToStringWithLimitedSpeed(filename, &s2, 20u << 20));
  double end_limited = WallTime_Now();
  EXPECT_GE((end_limited - begin_limited) * (20u << 20) * 1.1 + 4096, s1.size());
  ASSERT_EQ(s1, s2);
  unlink(filename.c_str());
}

TEST_F(FileUtilityTest, Test1) {
  static const int kTestSize = 100;
  std::vector<std::string> filenames;
  std::string s1("helloworld");
  std::string s2;
  for (int i = 0; i < kTestSize; ++i) {
    std::string filename;
    CreateTempFile(s1, &filename);
    filenames.push_back(filename);
  }

  std::vector<std::string> glob_names;

  Glob(
      "/tmp/file_utility_test_rand.*", 0, &glob_names);
  sort(filenames.begin(), filenames.end());
  std::vector<std::string>::iterator e = unique(filenames.begin(), filenames.end());
  filenames.erase(e, filenames.end());
  sort(glob_names.begin(), glob_names.end());
  ASSERT_EQ(filenames.size(), glob_names.size());
  for (size_t i = 0; i < filenames.size(); ++i) {
    ASSERT_EQ(filenames[i], glob_names[i]);
  }

  for (size_t i = 0; i < filenames.size(); ++i) {
    unlink(filenames[i].c_str());
  }
}

TEST_F(FileUtilityTest, Test2) {
  std::string s1("helloworld");
  std::string filename;
  CreateTempFile(s1, &filename);
  ASSERT_TRUE(FileExisting(filename));
  unlink(filename.c_str());
}

TEST_F(FileUtilityTest, TestChecksumFile) {
  std::string s1("helloworld");
  std::string filename;
  int i;
  for (i = 0; i < 1000; ++i) {
    s1 += "world";
  }
  CreateTempFile(s1, &filename);
  uLong init_adler = adler32(0L, Z_NULL, 0);
  ASSERT_EQ(ChecksumFile(filename),
            adler32(init_adler,
                    reinterpret_cast<const Bytef *>(s1.c_str()),
                    s1.length()));

  uint32_t offset = 10;
  int32_t len = 2148;
  std::string substr = s1.substr(offset, len);
  ASSERT_EQ(ChecksumFileOffset(filename, offset, len),
            adler32(init_adler,
                    reinterpret_cast<const Bytef *>(substr.c_str()),
                    substr.length()));

  substr = s1.substr(offset);
  ASSERT_EQ(ChecksumFileOffset(filename, offset, -1),
            adler32(init_adler,
                    reinterpret_cast<const Bytef *>(substr.c_str()),
                    substr.length()));
  unlink(filename.c_str());
}

TEST_F(FileUtilityTest, TestMakeUniquePath) {
  {
    char path[] = "/tmp/file_utility_test_XXXXXX";
    ASSERT_TRUE(MakeUniquePath(path, 0755));
    ASSERT_FALSE(FileExisting(path));
    system(StringPrintf("rm -rf %s", path).c_str());
  }
  {
    char path[] = "/tmp/file_utility_test_X";
    ASSERT_FALSE(MakeUniquePath(path, 0755));
  }
}

TEST_F(FileUtilityTest, TestGlobInDirs) {
  char test_path[] = "/tmp/file_utility_test_XXXXXX";
  ASSERT_TRUE(MakeUniquePath(test_path, 0755));
  LOG(INFO) << test_path;
  std::string path = test_path + std::string("/a/b/c/d");
  system(StringPrintf("rm -rf %s", test_path).c_str());

  ASSERT_TRUE(CreateMultiLevelPath(path, 0755));
  ASSERT_TRUE(FileExisting(test_path));
  std::string file1 = test_path + std::string("/a/1.txt");
  std::string file2 = test_path + std::string("/a/b/1.txt");
  std::string file3 = test_path + std::string("/a/b/c/1.txt");
  std::string file4 = test_path + std::string("/a/b/c/d/1.txt");
  std::string *files[] = {&file1, &file2, &file3, &file4, NULL};
  std::string cmd;
  for (int i = 0; files[i] != NULL; ++i) {
    cmd = std::string("touch ") + *files[i];
    system(cmd.c_str());
  }
  std::vector<std::string> result;
  std::vector<std::string>::size_type j;
  std::string pattern = test_path + std::string("/a/*.txt");
  ASSERT_TRUE(GlobInDirs(pattern, 0, &result));
  for (int i = 0; files[i] != NULL; ++i) {
    for (j = 0; j < result.size(); ++j) {
      if (result[j] == *files[i]) {
        break;
      }
    }
    ASSERT_NE(j, result.size());
  }
  system(StringPrintf("rm -rf %s", test_path).c_str());
}

TEST_F(FileUtilityTest, TestCreateMultiLevelPath) {
  {
    char test_path[] = "/tmp/file_util_test_XXXXXX";
    system(StringPrintf("rm -rf %s", test_path).c_str());
    ASSERT_TRUE(MakeUniquePath(test_path, 0755));
    std::string tmp_path(test_path);
    tmp_path = tmp_path + "/a.XXXXXX";
    ASSERT_TRUE(MakeUniquePath(&tmp_path[0], 0755));
    static const int32_t kPathSize = 255;
    char cur_path[kPathSize] = {0};
    getcwd(cur_path, kPathSize - 1);
    VLOG(2) << "cur_path: " << cur_path << " test_path: " << test_path;
    ASSERT_EQ(0, chdir(test_path));
    std::string path = "./a/b/c/d";
    ASSERT_TRUE(CreateMultiLevelPath(path, 0755));
    ASSERT_TRUE(FileExisting("./a"));
    ASSERT_TRUE(FileExisting("./a/b"));
    ASSERT_TRUE(FileExisting("./a/b/c"));
    ASSERT_TRUE(FileExisting("./a/b/c/d"));
    ASSERT_EQ(0, chdir(cur_path));
    system(StringPrintf("rm -rf %s", test_path).c_str());
  }
  {
    char test_path[] = "/tmp/file_util_test_XXXXXX";
    ASSERT_TRUE(MakeUniquePath(test_path, 0755));
    std::string path = test_path + std::string("/b/c/d");
    system(StringPrintf("rm -rf %s", test_path).c_str());
    ASSERT_TRUE(CreateMultiLevelPath(path, 0755));
    ASSERT_TRUE(FileExisting(test_path));
    ASSERT_TRUE(FileExisting(test_path + std::string("/b")));
    ASSERT_TRUE(FileExisting(test_path + std::string("/b/c")));
    ASSERT_TRUE(FileExisting(test_path + std::string("/b/c/d")));
    system(StringPrintf("rm -rf %s", test_path).c_str());
  }
}

TEST_F(FileUtilityTest, TestGetLastModifyFile) {
  std::string pre_file;
  CreateLastModifyTempFile("pre_file", &pre_file);
  sleep(2);
  std::string last_file;
  CreateLastModifyTempFile("last_file", &last_file);

  std::string search_pattern = "/tmp/last_modify_file_test_*";
  std::string result;
  ASSERT_TRUE(GetLastModifyFile(search_pattern, &result));
  unlink(pre_file.c_str());
  unlink(last_file.c_str());
  ASSERT_EQ(last_file, result);
}

TEST_F(FileUtilityTest, TestGetFileSize) {
  std::string s1("helloworld");
  std::string filename;
  CreateTempFile(s1, &filename);
  ASSERT_EQ(GetFileSize(filename), static_cast<int>(s1.size()));
  unlink(filename.c_str());
}

TEST_F(FileUtilityTest, TestMakesureFileExisting) {
  std::string s1("helloworld");
  std::string filename;
  CreateTempFile(s1, &filename);
  unlink(filename.c_str());
  filename += ".test";
  ASSERT_FALSE(FileExisting(filename));
  ASSERT_TRUE(MakesureFileExisting(filename, O_WRONLY| O_CREAT | O_TRUNC | O_LARGEFILE));
  ASSERT_TRUE(FileExisting(filename));
  unlink(filename.c_str());
}

TEST_F(FileUtilityTest, TestWriteStringToFile) {
  uint32_t offset;
  struct stat buff;
  std::string filename;
  std::string s1("helloworld");
  std::string readcontent;

  CreateTempFile("", &filename);
  EXPECT_TRUE(ReadFileToString(filename, &readcontent));
  EXPECT_EQ("", readcontent);

  ASSERT_TRUE(WriteStringToFile(filename, s1));
  EXPECT_TRUE(ReadFileOffsetToString(
      filename, 1, 4, &readcontent));
  EXPECT_EQ(s1.substr(1, 4), readcontent);

  readcontent = "";
  ASSERT_TRUE(ReadFileToString(filename, &readcontent));
  ASSERT_EQ(readcontent, s1);

  offset = 3;
  std::string s2("test for write file offset");
  ASSERT_TRUE(WriteStringToFileOffset(filename, s2, offset));
  readcontent.clear();
  ASSERT_TRUE(ReadFileOffsetToString(filename, offset, -1,
                                                     &readcontent));
  ASSERT_EQ(readcontent, s2);

  readcontent.clear();
  ASSERT_TRUE(ReadFileToString(filename, &readcontent));
  std::string content = s1.substr(0, offset) + s2;
  ASSERT_EQ(readcontent, content);

  ASSERT_TRUE(WriteStringToFileEnd(filename, s1));
  readcontent.clear();
  ASSERT_TRUE(ReadFileToString(filename, &readcontent));
  content += s1;
  ASSERT_EQ(readcontent, content);

  // try to write to offset that bigger than current file size
  offset = 50;
  ASSERT_TRUE(WriteStringToFileOffset(filename, s1, offset));
  ASSERT_GE(stat(filename.c_str(), &buff), 0);
  ASSERT_EQ(static_cast<int>(buff.st_size),
      static_cast<int>(offset + s1.size()));
  readcontent.clear();
  ASSERT_TRUE(ReadFileToString(filename, &readcontent));
  std::cout << readcontent << std::endl;

  unlink(filename.c_str());
}

TEST_F(FileUtilityTest, ReaStringFromFileTail) {
  std::string filename;
  std::string s1("\nhello\n world  hello  \n\n");
  CreateTempFile(s1, &filename);
  std::vector<std::string> lines;
  ASSERT_TRUE(ReadStringFromFileTail(filename, 4, &lines));
  ASSERT_EQ(lines.size(), 2u);
  ASSERT_EQ(lines[0], "hello");
  ASSERT_EQ(lines[1], " world  hello  ");
}

TEST_F(FileUtilityTest, ReaStringFromFileTailSingleLine1) {
  std::string filename;
  std::string s("hello world|436929629");
  CreateTempFile(s, &filename);
  std::vector<std::string> lines;
  ASSERT_TRUE(ReadStringFromFileTail(filename, 4, &lines));
  ASSERT_EQ(lines.size(), 1u);
  ASSERT_EQ(lines[0], s);
}

TEST_F(FileUtilityTest, ReaStringFromFileTailSingleLine2) {
  std::string filename;
  std::string s(529, 'c');
  CreateTempFile(s, &filename);
  std::vector<std::string> lines;
  ASSERT_TRUE(ReadStringFromFileTail(filename, 4, &lines));
  ASSERT_EQ(lines.size(), 1u);
  ASSERT_EQ(lines[0], s);
}

TEST_F(FileUtilityTest, ReaStringFromFileTailLargeLine) {
  std::string filename;
  std::string s(2000, 'a');
  CreateTempFile(s, &filename);
  std::vector<std::string> lines;
  ASSERT_TRUE(ReadStringFromFileTail(filename, 4, &lines));
  ASSERT_EQ(lines.size(), 1u);
  ASSERT_EQ(lines[0], s);
}

TEST_F(FileUtilityTest, ReaStringFromFileTailLarge) {
  std::string filename;
  std::string s1(530, 'a');
  std::string s2(1023, 'b');
  std::string s3(529, 'c');
  std::string s4(1, 'd');
  std::string s5(1, 'e');
  std::string s = "\n" + s1 + "\n" + s2 + "\n" + s3 + "\n" + s4 + "\n" + s5;
  CreateTempFile(s, &filename);
  std::vector<std::string> lines;
  ASSERT_TRUE(ReadStringFromFileTail(filename, 4, &lines));
  ASSERT_EQ(lines.size(), 4u);
  EXPECT_EQ(lines[0], s2);
  EXPECT_EQ(lines[1], s3);
  EXPECT_EQ(lines[2], s4);
  EXPECT_EQ(lines[3], s5);
}

TEST_F(FileUtilityTest, ReaStringFromFileNumber) {
  static const uint64_t start = 0xFFFF0000;
  static const uint64_t number = 0xFFFF;
  std::string filename;
  CreateTempFile("\n", &filename);
  for (uint64_t i = start; i < start + number; ++i) {
    WriteStringToFileEnd(filename, StringPrintf("%lu\n", i));
  }
  std::vector<std::string> lines;
  ASSERT_TRUE(ReadStringFromFileTail(filename, number, &lines));
  ASSERT_EQ(static_cast<uint64_t>(lines.size()), number);
  for (size_t i = 0; i < lines.size(); ++i) {
    EXPECT_EQ(lines[i], StringPrintf("%lu", start + i)) << i;
  }
}

TEST_F(FileUtilityTest, MD5FileTest) {
  std::string filename;
  ASSERT_TRUE(SafeWriteStringToMD5File("/tmp/md5filetest/hello-world", "helloworld"));
  std::string content;
  ASSERT_TRUE(SafeReadMD5FileToString("/tmp/md5filetest/hello-world-fc5e038d38a57032085441e7fe7010b0", &content));
  ASSERT_EQ(content, "helloworld");
  unlink("/tmp/md5filetest/hello-world-fc5e038d38a57032085441e7fe7010b0");
}

TEST_F(FileUtilityTest, MD5sumFileTest) {
  std::string filename;
  std::string sum;
  CreateTempFile("helloworld", &filename);
  ASSERT_TRUE(MD5sumFile(filename, &sum));
  ASSERT_EQ(sum, "fc5e038d38a57032085441e7fe7010b0");
}

TEST_F(FileUtilityTest, MD5sumFileWithLimitedSpeedTest) {
  std::string filename;
  std::string sum;
  std::string s1(1024 * 1024 * 100, '0');
  CreateTempFile(s1, &filename);
  double begin_time = WallTime_Now();
  ASSERT_TRUE(MD5sumFileWithLimitedSpeed(filename, &sum, 20 * 1024 * 1024));
  double end_time = WallTime_Now();
  EXPECT_GE((end_time - begin_time) * 20 * 1024 * 1024 * 1.1 + 4096, s1.size());
}

TEST_F(FileUtilityTest, MD5RenameFileTest) {
  std::string filename1, filename2;
  CreateTempFile("helloworld1", &filename1);
  CreateTempFile("helloworld2", &filename2);
  ASSERT_TRUE(MD5RenameFile(filename1, filename2));
  ASSERT_FALSE(MD5FileSanityCheck(filename1));
  ASSERT_TRUE(MD5FileSanityCheck(filename2 + "-b4190e42a5749f137b5e2e53c554dc71"));
}

TEST_F(FileUtilityTest, GetDirNameTest) {
  ASSERT_EQ(GetDirName("/a/b"), "/a");
  ASSERT_EQ(GetBaseName("/a/b"), "b");
}

TEST_F(FileUtilityTest, GetGetCurrentExecutePathTest) {
  ASSERT_GT(GetCurrentExecutePath().find("app/qzap/common/utility/file_utility_test"), 0u);
}

TEST_F(FileUtilityTest, IsCFSFile) {
  EXPECT_TRUE(IsCFSFile("/cfs"));
  EXPECT_TRUE(IsCFSFile("/cfs/release/snsad/qzap/svcdat/merger/"));
  EXPECT_TRUE(IsCFSFile("/cfs_sz_gray_2000010057"));
  EXPECT_FALSE(IsCFSFile(""));
  EXPECT_FALSE(IsCFSFile("./cfs"));
  EXPECT_FALSE(IsCFSFile("."));
  EXPECT_FALSE(IsCFSFile("."));
  EXPECT_FALSE(IsCFSFile("../cfs"));
}

TEST_F(FileUtilityTest, CopyFile) {
  std::string filename;
  std::string s(400, 'c');
  CreateTempFile(s, &filename);
  char dst_filename[] = "/tmp/file_utility_test_XXXXXX";
  ASSERT_TRUE(MakeUniquePath(dst_filename, 0755));
  ASSERT_TRUE(CopyFile(filename, dst_filename));
  std::string content;
  ASSERT_TRUE(ReadFileToString(dst_filename, &content));
  ASSERT_EQ(content, s);
}

TEST_F(FileUtilityTest, CopyFileToCfs) {
  std::string filename;
  std::string s(400, 'c');
  CreateTempFile(s, &filename);
  char dst_filename[] = "/tmp/file_utility_test_XXXXXX";
  ASSERT_TRUE(MakeUniquePath(dst_filename, 0755));
  ASSERT_TRUE(CopyFileToCfs(filename, dst_filename, 10000));
  std::string content;
  ASSERT_TRUE(ReadFileToString(dst_filename, &content));
  ASSERT_EQ(content, s);
}



