// Copyright (c) 2015 Tencent Inc.
// Author: Li Meng (elvisli@tencent.com)

#ifndef COMMON_RANDOM_PLAYBACK_RANDOM_H_
#define COMMON_RANDOM_PLAYBACK_RANDOM_H_

#include <deque>

#include "app/qzap/common/utility/true_random.h"
#include "common/system/concurrency/mutex.h"

namespace gdt {

class PlaybackRandom {
 public:
  // random_value can be null, empty and non-empty;
  // Null means Generate mode;
  // Empty means Record mode;
  // Non-empty means Playback mode.
  // In Record and Playback mode, the life of random_record
  // should be longer than object life of PlaybackRandom
  explicit PlaybackRandom(std::deque<uint32_t>* random_record);
  ~PlaybackRandom();

  // return random integer in range [0, UINT_MAX]
  uint32_t NextUInt32();

  // return random integer in range [0, max_value)
  uint32_t NextUInt32(uint32_t max_value);

  // return double in range [0.0,1.0]
  double NextDouble();

 private:
  enum WorkingMode { kGenerate, kRecord, kPlayback };

  Mutex mutex_;
  WorkingMode working_mode_;
  std::deque<uint32_t>* random_record_;
  static TrueRandom true_random_;
};

}  // namespace gdt

#endif  // COMMON_RANDOM_PLAYBACK_RANDOM_H_
