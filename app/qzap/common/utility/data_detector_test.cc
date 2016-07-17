// Copyright (c) 2015, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>
#include "app/qzap/common/utility/data_detector.h"
#include "thirdparty/gtest/gtest.h"

#include "app/qzap/common/utility/file_utility.h"

namespace gdt {

TEST(DataDetectorTest, simple) {
  DataDetector detector("data_*");
  EXPECT_FALSE(detector.TryDetectingNewestData());
  ASSERT_TRUE(WriteStringToFile("data_001", "abc"));
  ASSERT_TRUE(detector.TryDetectingNewestData());
  ASSERT_EQ(std::string("data_001"), detector.GetCurrentName());
  shared_ptr<ScopedMMap> memory1 = detector.GetCurrentMemory();
  ASSERT_EQ(std::string("abc"), std::string(memory1->ptr(), memory1->size()));
  EXPECT_FALSE(detector.TryDetectingNewestData());
  ASSERT_EQ(std::string("abc"), std::string(memory1->ptr(), memory1->size()));

  ASSERT_TRUE(WriteStringToFile("data_002", "efg"));
  ASSERT_TRUE(detector.TryDetectingNewestData());
  ASSERT_EQ(std::string("data_002"), detector.GetCurrentName());
  shared_ptr<ScopedMMap> memory2 = detector.GetCurrentMemory();
  ASSERT_EQ(std::string("efg"), std::string(memory2->ptr(), memory2->size()));
  EXPECT_FALSE(detector.TryDetectingNewestData());
  ASSERT_EQ(std::string("efg"), std::string(memory2->ptr(), memory2->size()));
}

}  // namespace gdt
