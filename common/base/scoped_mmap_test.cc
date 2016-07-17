// Copyright (c) 2015, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <string>

#include "common/base/scoped_mmap.h"
#include "common/base/scoped_ptr.h"
#include "thirdparty/gtest/gtest.h"
namespace gdt {

TEST(ScopedMMapTest, ScopedMMap) {
  std::string name = "scoped_mmap_test.txt";
  std::string content = "abcdefg";

  int32_t fd = open(name.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
  ASSERT_NE(-1, fd);
  int32_t n = pwrite(fd, content.c_str(), content.size(), 0);
  ASSERT_EQ(content.size(), n);

  void* ptr = mmap(0, n, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);

  if (ptr == MAP_FAILED) {
    PLOG(ERROR) << "Mapp failed";
  }
  ASSERT_FALSE(ptr == MAP_FAILED);

  scoped_ptr<ScopedMMap> memory(new ScopedMMap(ptr, n));
  ASSERT_EQ(n, memory->size());
  ASSERT_EQ(content, std::string(memory->ptr(), n));
  memory.reset();
}
}  // namespace gdt
