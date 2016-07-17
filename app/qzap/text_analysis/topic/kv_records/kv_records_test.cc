// Copyright (c) 2013 Tencent Inc.
// Author: Wang Yi (yiwang@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/kv_records/kv_records.h"

#include <fstream>

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

namespace qzap {
namespace text_analysis {
namespace base {

TEST(KVRecord, ReadAndWrite) {
  const std::string kFilename = "./testdata/records.kv";
  std::ofstream fout(kFilename.c_str());
  KVRecordWriter w(&fout);
  EXPECT_EQ(0, w.Write("apple", "100"));
  EXPECT_EQ(0, w.Write("orange", "200"));
  w.Flush();
  fout.close();

  std::ifstream fin(kFilename.c_str());
  KVRecordReader r(&fin);
  std::string key, value;
  EXPECT_EQ(0, r.Read(&key, &value));
  EXPECT_EQ("apple", key);
  EXPECT_EQ("100", value);
  EXPECT_EQ(0, r.Read(&key, &value));
  EXPECT_EQ("orange", key);
  EXPECT_EQ("200", value);
  EXPECT_EQ(-1, r.Read(&key, &value));
  fin.close();
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);

  return RUN_ALL_TESTS();
}
