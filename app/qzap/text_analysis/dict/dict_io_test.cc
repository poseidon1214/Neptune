// Copyright (c) 2011 Tencent Inc.
// Author: Zhao Xuemin (xueminzhao@tencent.com)
//         Pan Yang (baileyyang@tencent.com)
//         Lifeng Wang (fandywang@tencent.com)

#include "app/qzap/text_analysis/dict/dict_io.h"

#include <time.h>
#include <fstream>

#include "thirdparty/gtest/gtest.h"

#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/text_analysis/dict/dict.pb.h"

using std::tr1::shared_ptr;
using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using qzap::text_analysis::ReadString;
using qzap::text_analysis::WriteString;
using qzap::text_analysis::ReadProtobufMessages;
using qzap::text_analysis::WriteProtobufMessages;
using qzap::text_analysis::ReadPOD;
using qzap::text_analysis::WritePOD;
using qzap::text_analysis::IdfInfo;
typedef IdfInfo* IdfInfoPtr;
typedef shared_ptr<IdfInfo> IdfInfoSharedPtr;

struct PODStruct {
  static const uint32_t array_len_ = 100;

  int32_t i_;
  char array_[array_len_];
};  // struct PODStruct

inline void DeleteAndClear(vector<IdfInfoPtr> *vec) {
  for (size_t i = 0; i < vec->size(); ++i) {
    delete (*vec)[i];
  }
  vec->clear();
}

TEST(DictIOTest, ReadWriteString) {
  const string filename = "dictio_test.bin";

  string write_str1, write_str2 = "太好啦";

  ofstream out(filename.c_str(), ios_base::binary);
  ASSERT_FALSE(!out);
  WriteString(write_str1, out);
  WriteString(write_str2, out);
  out.close();

  string read_str1, read_str2;

  ifstream in(filename.c_str(), ios_base::binary);
  ASSERT_FALSE(!in);
  ReadString(in, &read_str1);
  ReadString(in, &read_str2);
  in.close();

  EXPECT_EQ(write_str1, read_str1);
  EXPECT_EQ(write_str2, read_str2);
}

TEST(DictIOTest, ReadWriteProtobufMessages) {
  const string filename = "dictio_test.bin";

  srand(time(NULL));

  uint32_t len = rand() % 100 + 100;
  vector<IdfInfo> write_msgs_value;
  vector<IdfInfoPtr> write_msgs_ptr;
  vector<IdfInfoSharedPtr> write_msgs_sharedptr;
  write_msgs_value.reserve(len);
  write_msgs_ptr.reserve(len);
  write_msgs_sharedptr.reserve(len);

  IdfInfo idfinfo;
  for (uint32_t i = 0; i < len; ++i) {
    idfinfo.set_idf_commercial(static_cast<float>(rand()) /
                               static_cast <float> (RAND_MAX));
    write_msgs_value.push_back(idfinfo);
    write_msgs_ptr.push_back(new IdfInfo(idfinfo));
    write_msgs_sharedptr.push_back(IdfInfoSharedPtr(new IdfInfo(idfinfo)));
  }

  ofstream out(filename.c_str(), ios_base::binary);
  ASSERT_FALSE(!out);
  WriteProtobufMessages(write_msgs_value.begin(), write_msgs_value.end(), out);
  WriteProtobufMessages(write_msgs_ptr.begin(), write_msgs_ptr.end(), out);
  WriteProtobufMessages(write_msgs_sharedptr.begin(),
                        write_msgs_sharedptr.end(),
                        out);
  out.close();

  vector<IdfInfo> read_msgs_value;
  vector<IdfInfoPtr> read_msgs_ptr;
  vector<IdfInfoSharedPtr> read_msgs_sharedptr;

  ifstream in(filename.c_str(), ios_base::binary);
  ASSERT_FALSE(!in);
  ReadProtobufMessages(in, &read_msgs_value);
  ReadProtobufMessages(in, &read_msgs_ptr);
  ReadProtobufMessages(in, &read_msgs_sharedptr);
  in.close();

  ASSERT_EQ(write_msgs_value.size(), read_msgs_value.size());
  ASSERT_EQ(write_msgs_ptr.size(), read_msgs_ptr.size());
  ASSERT_EQ(write_msgs_sharedptr.size(), read_msgs_sharedptr.size());
  for (uint32_t i = 0; i < len; ++i) {
    EXPECT_EQ(write_msgs_value[i].DebugString(),
              read_msgs_value[i].DebugString());
    EXPECT_EQ(write_msgs_ptr[i]->DebugString(),
              read_msgs_ptr[i]->DebugString());
    EXPECT_EQ(write_msgs_sharedptr[i]->DebugString(),
              read_msgs_sharedptr[i]->DebugString());
  }

  ASSERT_EQ(write_msgs_value.size(), read_msgs_value.size());
  ASSERT_EQ(write_msgs_ptr.size(), read_msgs_ptr.size());
  ASSERT_EQ(write_msgs_sharedptr.size(), read_msgs_sharedptr.size());
  for (uint32_t i = 0; i < len; ++i) {
    EXPECT_EQ(write_msgs_value[i].DebugString(),
              read_msgs_value[i].DebugString());
    EXPECT_EQ(write_msgs_ptr[i]->DebugString(),
              read_msgs_ptr[i]->DebugString());
    EXPECT_EQ(write_msgs_sharedptr[i]->DebugString(),
              read_msgs_sharedptr[i]->DebugString());
  }

  DeleteAndClear(&write_msgs_ptr);
  DeleteAndClear(&read_msgs_ptr);
}

TEST(DictIOTest, ReadWritePOD) {
  const string filename = "dictio_test.bin";

  srand(time(NULL));

  PODStruct write_pod;
  write_pod.i_ = rand();

  ofstream out(filename.c_str());
  ASSERT_FALSE(!out);
  WritePOD(write_pod, out);
  out.close();

  PODStruct read_pod;

  ifstream in(filename.c_str());
  ASSERT_FALSE(!in);
  ReadPOD(in, &read_pod);
  in.close();

  EXPECT_EQ(write_pod.i_, read_pod.i_);
  for (uint32_t i = 0; i < PODStruct::array_len_; ++i) {
    EXPECT_EQ(write_pod.array_[i], read_pod.array_[i]);
  }
}

