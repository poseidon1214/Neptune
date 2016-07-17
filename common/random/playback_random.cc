// Copyright (c) 2015 Tencent Inc.
// Author: Li Meng (elvisli@tencent.com)

#include "common/random/playback_random.h"

#include <limits.h>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_uint64(random_record_max_num, 1000000, "max number of random record");

namespace gdt {

TrueRandom PlaybackRandom::true_random_;

PlaybackRandom::PlaybackRandom(
  std::deque<uint32_t>* random_record) {
  if (random_record == NULL) {
    working_mode_ = kGenerate;
  } else if (random_record->empty()) {
    working_mode_ = kRecord;
  } else {
    working_mode_ = kPlayback;
  }
  random_record_ = random_record;
}

PlaybackRandom::~PlaybackRandom() {
}

uint32_t PlaybackRandom::NextUInt32() {
  uint32_t random_value = 0;
  if (working_mode_ == kGenerate) {
    random_value = true_random_.NextUInt32();
  } else if (working_mode_ == kRecord) {
    random_value = true_random_.NextUInt32();
    MutexLocker locker(mutex_);
    if (random_record_->size() < FLAGS_random_record_max_num)
      random_record_->push_back(random_value);
  } else if (working_mode_ == kPlayback) {
    MutexLocker locker(mutex_);
    if (!random_record_->empty()) {
      random_value = random_record_->front();
      random_record_->pop_front();
    } else {
      random_value = true_random_.NextUInt32();
      LOG(ERROR) << "Playback mode generate new random value: "
                 << random_value;
    }
  }
  return random_value;
}

uint32_t PlaybackRandom::NextUInt32(uint32_t max_value) {
  return NextUInt32() % max_value;
}

double PlaybackRandom::NextDouble() {
  return static_cast<double>(NextUInt32()) / UINT32_MAX;
}

}  // namespace gdt

