// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/benchmark.h"
#include "app/qzap/common/utility/hash.h"
#include "app/qzap/common/utility/pod_hash.h"
#include "app/qzap/common/base/shared_ptr.h"
#include "thirdparty/gtest/gtest.h"
class PodHashTest : public testing::Test {
};

typedef char Key[256];
typedef struct {
  int64_t a;
  int64_t b;
} Value;

TEST_F(PodHashTest, Test1) {
  VLOG(2) << "pod: " << std::tr1::is_pod<Value>::value;
  typedef PodHash<Key, Value> TestHash;
  TestHash pod;
  const int buffer_size = sizeof(TestHash::Header) + sizeof(TestHash::DataBlock) * 2 + sizeof(TestHash::Bucket) * 4;
  std::string buf(buffer_size, '\0');
  pod.Init(&buf[0], buf.size(), 2);
  Key key1 = "helloworld1";
  Key key2 = "helloworld11";
  Key key3 = "helloworld111";
  Value value1, value2;
  Value value3, value4;
  value1.a = 1;
  value1.b = 2;
  value2.a = 3;
  value2.b = 4;
  ASSERT_TRUE(pod.Add(key1, value1, false));
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_FALSE(pod.IsFull());
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_TRUE(pod.Add(key2, value2, false));
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_TRUE(pod.IsFull());
  
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_TRUE(pod.Find(key2, &value4));
  ASSERT_EQ(value3.a, 1);
  ASSERT_EQ(value3.b, 2);
  ASSERT_EQ(value4.a, 3);
  ASSERT_EQ(value4.b, 4);
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_TRUE(pod.Delete(key1));
  ASSERT_FALSE(pod.Find(key1, &value3));
  value1.a = 10;
  value1.b = 11;
  ASSERT_TRUE(pod.Add(key1, value1, false));
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_EQ(value3.a, 10);
  ASSERT_EQ(value3.b, 11);
  ASSERT_FALSE(pod.Add(key1, value1, false));
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_EQ(value3.a, 10);
  ASSERT_EQ(value3.b, 11);

  value1.a = 12;
  value1.b = 13;
  ASSERT_TRUE(pod.Delete(key1));
  ASSERT_TRUE(pod.Add(key3, value1, true));
  ASSERT_TRUE(pod.Find(key3, &value3));
  ASSERT_EQ(value3.a, 12);
  ASSERT_EQ(value3.b, 13);

  std::string buf2;
  for (size_t i = 0; i < buf.size(); ++i) {
    buf2.push_back(buf[i]);
  }

  TestHash hash2;
  ASSERT_TRUE(hash2.Mount(&buf2[0], buf2.size()));
  ASSERT_FALSE(hash2.Find(key1, &value3));
  ASSERT_TRUE(hash2.Find(key2, &value4));
  ASSERT_TRUE(hash2.Find(key3, &value3));
  ASSERT_EQ(value4.a, 3);
  ASSERT_EQ(value4.b, 4);
  ASSERT_EQ(value3.a, 12);
  ASSERT_EQ(value3.b, 13);
  ASSERT_FALSE(hash2.Delete(key1));
  ASSERT_TRUE(hash2.Delete(key2));
 
  buf.clear();
  for (size_t i = 0; i < buf2.size(); ++i) {
    buf.push_back(buf2[i]);
  }
  ASSERT_TRUE(pod.Mount(&buf[0], buf.size()));
  ASSERT_FALSE(pod.Find(key1, &value3));
  ASSERT_FALSE(pod.Find(key2, &value4));
  ASSERT_TRUE(pod.Find(key3, &value3));

  // destroy the memory
  for (size_t i = buf.size() / 2; i < buf.size(); ++i) {
    buf[i] = 'a';
  }
  ASSERT_TRUE(pod.Mount(&buf[0], buf.size()));
  ASSERT_FALSE(pod.Find(key1, &value3));
  ASSERT_FALSE(pod.Find(key2, &value4));
  ASSERT_FALSE(pod.Find(key3, &value3));
}

TEST_F(PodHashTest, Test2) {
  VLOG(2) << "pod: " << std::tr1::is_pod<Value>::value;
  typedef PodHash<uint32_t, Value> TestHash;
  TestHash pod;
  const int buffer_size = sizeof(TestHash::Header) + sizeof(TestHash::DataBlock) * 2 + sizeof(TestHash::Bucket) * 4;
  std::string buf(buffer_size, '\0');
  pod.Init(&buf[0], buf.size(), 2);
  uint32_t key1 = 1;
  uint32_t key2 = 2;
  uint32_t key3 = 3;
  Value value1, value2;
  Value value3, value4;
  value1.a = 1;
  value1.b = 2;
  value2.a = 3;
  value2.b = 4;
  ASSERT_TRUE(pod.Add(key1, value1, false));
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_FALSE(pod.IsFull());
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_TRUE(pod.Add(key2, value2, false));
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_TRUE(pod.IsFull());
  
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_TRUE(pod.Find(key2, &value4));
  ASSERT_EQ(value3.a, 1);
  ASSERT_EQ(value3.b, 2);
  ASSERT_EQ(value4.a, 3);
  ASSERT_EQ(value4.b, 4);
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_TRUE(pod.Delete(key1));
  ASSERT_FALSE(pod.Find(key1, &value3));
  value1.a = 10;
  value1.b = 11;
  ASSERT_TRUE(pod.Add(key1, value1, false));
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_EQ(value3.a, 10);
  ASSERT_EQ(value3.b, 11);
  ASSERT_FALSE(pod.Add(key1, value1, false));
  ASSERT_TRUE(pod.Find(key1, &value3));
  ASSERT_EQ(value3.a, 10);
  ASSERT_EQ(value3.b, 11);

  value1.a = 12;
  value1.b = 13;
  ASSERT_TRUE(pod.Delete(key1));
  ASSERT_TRUE(pod.Add(key3, value1, true));
  ASSERT_TRUE(pod.Find(key3, &value3));
  ASSERT_EQ(value3.a, 12);
  ASSERT_EQ(value3.b, 13);

  std::string buf2;
  for (size_t i = 0; i < buf.size(); ++i) {
    buf2.push_back(buf[i]);
  }

  TestHash hash2;
  ASSERT_TRUE(hash2.Mount(&buf2[0], buf2.size()));
  ASSERT_FALSE(hash2.Find(key1, &value3));
  ASSERT_TRUE(hash2.Find(key2, &value4));
  ASSERT_TRUE(hash2.Find(key3, &value3));
  ASSERT_EQ(value4.a, 3);
  ASSERT_EQ(value4.b, 4);
  ASSERT_EQ(value3.a, 12);
  ASSERT_EQ(value3.b, 13);
  ASSERT_FALSE(hash2.Delete(key1));
  ASSERT_TRUE(hash2.Delete(key2));
 
  buf.clear();
  for (size_t i = 0; i < buf2.size(); ++i) {
    buf.push_back(buf2[i]);
  }
  ASSERT_TRUE(pod.Mount(&buf[0], buf.size()));
  ASSERT_FALSE(pod.Find(key1, &value3));
  ASSERT_FALSE(pod.Find(key2, &value4));
  ASSERT_TRUE(pod.Find(key3, &value3));

  // destroy the memory
  for (size_t i = buf.size() / 2; i < buf.size(); ++i) {
    buf[i] = 'a';
  }
  ASSERT_TRUE(pod.Mount(&buf[0], buf.size()));
  ASSERT_FALSE(pod.Find(key1, &value3));
  ASSERT_FALSE(pod.Find(key2, &value4));
  ASSERT_FALSE(pod.Find(key3, &value3));
}

TEST_F(PodHashTest, Test3) {
  VLOG(2) << "pod: " << std::tr1::is_pod<Value>::value;
  typedef PodHash<uint32_t, Value> TestHash;
  TestHash pod;
  const int buffer_size = sizeof(TestHash::Header) + sizeof(TestHash::DataBlock) * 2 + sizeof(TestHash::Bucket) * 4;
  std::string buf(buffer_size, '\0');
  pod.Init(&buf[0], buf.size(), 2);
  uint32_t key1 = 1;
  uint32_t key2 = 2;
  Value value1, value2;
  value1.a = 1;
  value1.b = 2;
  value2.a = 3;
  value2.b = 4;
  ASSERT_EQ(pod.FreeItemNumber(), 2u);
  ASSERT_TRUE(pod.Add(key1, value1, false));
  ASSERT_EQ(pod.FreeItemNumber(), 1u);
  ASSERT_TRUE(pod.Add(key2, value2, false));
  ASSERT_EQ(pod.FreeItemNumber(), 0u);
}
