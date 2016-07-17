// Copyright 2011 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Zhihui Jin (rickjin@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)
//
// Wrappers for several pseudo-random number generators used in LDA, including
// the default unix RNG(random number generator), and RNGs provided in Boost
// library.  We suggest not using unix RNG due to its poor randomness in
// application.

#ifndef APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_RANDOM_H_
#define APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_RANDOM_H_

#include <stdint.h>
#include <stdlib.h>

namespace qzap {
namespace text_analysis {
namespace base {

// The RNG wrapper interface
class Random {
 public:
  Random() {}

  virtual ~Random() {}

  // Seed the RNG using specified seed or current time(if seed < 0).
  // In order to achieve maximum randomness we use current time in
  // millisecond as the seed.  Note that it is not a good idea to
  // seed with current time in second when multiple random number
  // sequences are required, which usually produces correlated number
  // sequences and results in poor randomness.
  virtual void SeedRNG(int32_t seed) = 0;

  // Generate a random float value in the range of [0,1) from the
  // uniform distribution.
  virtual double RandDouble() = 0;

  // Generate a random integer value in the range of [0,bound) from the
  // uniform distribution.
  virtual int32_t RandInt32(int32_t bound) {
    return static_cast<int32_t>(RandDouble() * bound);
  }

  // Get tick count of the day, used as random seed
  static uint32_t GetTickCount();
};  // class Random


// The MT19937 algorithm stolen from http://www.agner.org/random/.
class MTRandom : public Random {
 public:
  MTRandom() { SeedRNG(0); }

  virtual void SeedRNG(int seed);
  virtual double RandDouble();

 private:
  static const uint32_t MERS_N = 624;
  static const uint32_t MERS_M = 397;
  static const uint32_t MERS_R = 31;
  static const uint32_t MERS_U = 11;
  static const uint32_t MERS_S = 7;
  static const uint32_t MERS_T = 15;
  static const uint32_t MERS_L = 18;
  static const uint32_t MERS_A = 0x9908B0DF;
  static const uint32_t MERS_B = 0x9D2C5680;
  static const uint32_t MERS_C = 0xEFC60000;

  uint32_t Uint32();

  uint32_t mt[MERS_N];  // state vector
  uint32_t mti;         // index into mt
};

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_TOPIC_BASE_RANDOM_H_
