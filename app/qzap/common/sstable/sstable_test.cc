// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-28
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/sstable/sstable.h"
#include "app/qzap/common/utility/file_utility.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/leveldb/env.h"
class SSTableTest : public testing::Test {
 public:
  void SetUp() {
  }

  void TearDown() {
    system("rm -rf /tmp/sstable_test.*");
  }

  void CreateTempFileName(std::string *filename) {
    char local_filename[] = "/tmp/sstable_test.XXXXXX";
    int fd = mkstemp(local_filename);
    CHECK_NE(fd, -1);
    close(fd);
    *filename = local_filename;
  }
};

TEST_F(SSTableTest, Test1) {
  static const int kTestNum = 1000;
  std::string filename;
  CreateTempFileName(&filename);
  SSTableBuilder builder(filename);
  unlink(filename.c_str());

  for (int i = 0; i < kTestNum; ++i) {
    ASSERT_TRUE(builder.Add(
      StringPrintf("helloworld%d", i),
      StringPrintf("data%d", i)));
  }
  ASSERT_EQ(static_cast<int>(builder.NumEntries()), kTestNum);
  ASSERT_TRUE(builder.Build());
  scoped_ptr<SSTable> table(SSTable::Open(filename));
  ASSERT_TRUE(table != NULL);
  scoped_ptr<SSTable::Iterator> it(table->NewIterator());
  ASSERT_TRUE(it != NULL);
  ASSERT_TRUE(it->Valid());
  for (int i = 0; i < kTestNum; ++i) {
    ASSERT_EQ(
      it->key(),
      StringPrintf("helloworld%d", i));
    ASSERT_EQ(
      it->value(),
      StringPrintf("data%d", i));
    it->Next();
    if (i < kTestNum - 1) {
      ASSERT_TRUE(it->Valid()) << i;
    }
  }
  ASSERT_FALSE(it->Valid());
  it->SeekToLast();
  for (int i = kTestNum - 1; i >=0; --i) {
    ASSERT_EQ(
      it->key(),
      StringPrintf("helloworld%d", i));
    ASSERT_EQ(
      it->value(),
      StringPrintf("data%d", i));
    it->Prev();
    if (i > 0) {
      ASSERT_TRUE(it->Valid());
    }
  }
}

TEST_F(SSTableTest, Test2) {
  std::string filename;
  CreateTempFileName(&filename);
  SSTableBuilder builder(filename);
  unlink(filename.c_str());

  builder.Add("2", "2");
  builder.Add("1", "1");
  builder.Add("3", "3");
  builder.Add("", "4");
  ASSERT_TRUE(builder.Build());
  scoped_ptr<SSTable> table(SSTable::Open(filename));
  ASSERT_TRUE(table != NULL);
  scoped_ptr<SSTable::Iterator> it(table->NewIterator());
  ASSERT_TRUE(it != NULL);
  ASSERT_TRUE(it->Valid());
  ASSERT_EQ(it->key(), "2");
  ASSERT_EQ(it->value(), "2");
  it->Next();
  ASSERT_EQ(it->key(), "1");
  ASSERT_EQ(it->value(), "1");
  it->Next();
  ASSERT_EQ(it->key(), "3");
  ASSERT_EQ(it->value(), "3");
  it->Next();
  ASSERT_EQ(it->key(), "");
  ASSERT_EQ(it->value(), "4");
}

TEST_F(SSTableTest, TestFromMemoryString) {
  std::string filename;
  CreateTempFileName(&filename);
  SSTableBuilder builder(filename);
  unlink(filename.c_str());

  builder.Add("2", "2");
  builder.Add("1", "1");
  builder.Add("3", "3");
  builder.Add("", "4");
  ASSERT_TRUE(builder.Build());

  std::string input;
  ASSERT_TRUE(ReadFileToString(filename, &input));
  scoped_ptr<SSTable> table(SSTable::OpenFromString(input));
  ASSERT_TRUE(table != NULL);
  scoped_ptr<SSTable::Iterator> it(table->NewIterator());
  ASSERT_TRUE(it != NULL);
  ASSERT_TRUE(it->Valid());
  ASSERT_EQ("2", it->key());
  ASSERT_EQ("2", it->value());
  it->Next();
  ASSERT_EQ("1", it->key());
  ASSERT_EQ("1", it->value());
  it->Next();
  ASSERT_EQ("3", it->key());
  ASSERT_EQ("3", it->value());
  it->Next();
  ASSERT_EQ("", it->key());
  ASSERT_EQ("4", it->value());
}

TEST_F(SSTableTest, TestFromMemoryArray) {
  std::string filename;
  CreateTempFileName(&filename);
  SSTableBuilder builder(filename);
  unlink(filename.c_str());

  builder.Add("2", "2");
  builder.Add("1", "1");
  builder.Add("3", "3");
  builder.Add("", "4");
  ASSERT_TRUE(builder.Build());

  std::string input;
  ASSERT_TRUE(ReadFileToString(filename, &input));
  scoped_ptr<SSTable> table(SSTable::OpenFromArray(input.data(), input.size()));
  ASSERT_TRUE(table != NULL);
  scoped_ptr<SSTable::Iterator> it(table->NewIterator());
  ASSERT_TRUE(it != NULL);
  while (it->Valid()) {
    LOG(INFO) << "key: " << it->key() << " value: " << it->value();
    it->Next();
  }
  it->SeekToFirst();
  ASSERT_TRUE(it->Valid());
  ASSERT_EQ("2", it->key());
  ASSERT_EQ("2", it->value());
  it->Next();
  ASSERT_EQ("1", it->key());
  ASSERT_EQ("1", it->value());
  it->Next();
  ASSERT_EQ("3", it->key());
  ASSERT_EQ("3", it->value());
  it->Next();
  ASSERT_EQ("", it->key());
  ASSERT_EQ("4", it->value());
}

TEST_F(SSTableTest, TestSnappyCompress) {
  std::string filename;
  CreateTempFileName(&filename);
  leveldb::Options options;
  options.create_if_missing = true;
  options.error_if_exists = true;
  options.compression = leveldb::kSnappyCompression;
  SSTableBuilder builder(filename, options);
  unlink(filename.c_str());

  builder.Add("2", "2");
  builder.Add("1", "1");
  builder.Add("3", "3");
  builder.Add("", "4");
  ASSERT_TRUE(builder.Build());
  scoped_ptr<SSTable> table(SSTable::Open(filename));
  ASSERT_TRUE(table != NULL);
  scoped_ptr<SSTable::Iterator> it(table->NewIterator());
  ASSERT_TRUE(it != NULL);
  ASSERT_TRUE(it->Valid());
  ASSERT_EQ(it->key(), "2");
  ASSERT_EQ(it->value(), "2");
  it->Next();
  ASSERT_EQ(it->key(), "1");
  ASSERT_EQ(it->value(), "1");
  it->Next();
  ASSERT_EQ(it->key(), "3");
  ASSERT_EQ(it->value(), "3");
  it->Next();
  ASSERT_EQ(it->key(), "");
  ASSERT_EQ(it->value(), "4");
}
