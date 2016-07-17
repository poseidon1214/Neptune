// Copyright (c) 2015 Tencent Inc.
// Author: Li Meng (elvisli@tencent.com)

#include "common/random/playback_random.h"

#include <limits.h>

#include "common/base/scoped_ptr.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

TEST(PlaybackRandom, GenerateAndReproduce) {
  // Generate mode
  scoped_ptr<PlaybackRandom> playback_random(
      new PlaybackRandom(NULL));
  uint32_t random_value_1 = playback_random->NextUInt32();
  EXPECT_GE(random_value_1, 0);
  EXPECT_LE(random_value_1, UINT32_MAX);
  uint32_t random_value_2 = playback_random->NextUInt32(100);
  EXPECT_GE(random_value_2, 0);
  EXPECT_LT(random_value_2, 100);
  double random_value_3 = playback_random->NextDouble();
  EXPECT_GE(random_value_3, 0);
  EXPECT_LE(random_value_3, 1.0);

  // Record mode
  std::deque<uint32_t> random_record;
  playback_random.reset(new PlaybackRandom(&random_record));
  random_value_1 = playback_random->NextUInt32();
  EXPECT_GE(random_value_1, 0);
  EXPECT_LE(random_value_1, UINT32_MAX);
  random_value_2 = playback_random->NextUInt32(100);
  EXPECT_GE(random_value_2, 0);
  EXPECT_LT(random_value_2, 100);
  random_value_3 = playback_random->NextDouble();
  EXPECT_GE(random_value_3, 0);
  EXPECT_LE(random_value_3, 1.0);

  // Playback mode
  playback_random.reset(new PlaybackRandom(&random_record));
  EXPECT_EQ(random_value_1, playback_random->NextUInt32());
  EXPECT_EQ(random_value_2, playback_random->NextUInt32(100));
  EXPECT_DOUBLE_EQ(random_value_3, playback_random->NextDouble());
  double random_value_4 = playback_random->NextDouble();
  EXPECT_GE(random_value_4, 0);
  EXPECT_LE(random_value_4, 1.0);
}

}  // namespace gdt
