// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//

#include "common/cache/memory_cache.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

TEST(MemoryCache, Capacity) {
  MemoryCache<int, int> cache(3, 0);
  EXPECT_EQ(3U, cache.Capacity());
}


TEST(MemoryCache, Size) {
  MemoryCache<int, int> cache(3, 0);
  EXPECT_EQ(0U, cache.Size());
  EXPECT_TRUE(cache.IsEmpty());

  cache.Put(1, 0);
  EXPECT_EQ(1U, cache.Size());
  EXPECT_FALSE(cache.IsEmpty());
  EXPECT_FALSE(cache.IsFull());

  cache.Put(2, 0);
  EXPECT_EQ(2U, cache.Size());
  EXPECT_FALSE(cache.IsEmpty());
  EXPECT_FALSE(cache.IsFull());

  cache.Put(3, 0);
  EXPECT_EQ(3U, cache.Size());
  EXPECT_FALSE(cache.IsEmpty());
  EXPECT_TRUE(cache.IsFull());

  cache.Put(4, 0);
  EXPECT_EQ(3U, cache.Size());
  EXPECT_FALSE(cache.IsEmpty());
  EXPECT_TRUE(cache.IsFull());
}

TEST(MemoryCache, PutAndGetWithoutExpiry) {
  MemoryCache<int, int> cache(3, 100);
  int value = 0;
  EXPECT_TRUE(cache.Put(1, 1));
  EXPECT_TRUE(cache.Get(1, &value));
  EXPECT_EQ(1, value);
  sleep(1);
  EXPECT_TRUE(cache.Put(1, 2));
  EXPECT_TRUE(cache.Get(1, &value));
  EXPECT_EQ(2, value);
  EXPECT_TRUE(cache.Put(2, 1));
  EXPECT_TRUE(cache.Get(2, &value));
  EXPECT_EQ(1, value);
  EXPECT_TRUE(cache.Put(3, 1));
  EXPECT_TRUE(cache.Get(3, &value));
  EXPECT_EQ(1, value);
  EXPECT_TRUE(cache.Put(4, 1));
  EXPECT_TRUE(cache.Get(4, &value));
  EXPECT_EQ(1, value);
  EXPECT_FALSE(cache.Get(1, &value));
  EXPECT_TRUE(cache.Put(5, 1));
}


TEST(MemoryCache, PutWithExpiry) {
  MemoryCache<int, int> cache(3, 0);
  EXPECT_TRUE(cache.Put(1, 1));
  sleep(1);
  EXPECT_FALSE(cache.Contains(1));
}

}  // namespace gdt
