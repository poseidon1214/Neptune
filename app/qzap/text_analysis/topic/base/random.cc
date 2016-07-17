// Copyright 2010 Tencent Inc.
// Author: Yi Wang (yiwang@tencent.com)
//         Zhihui Jin (rickjin@tencent.com)
//         Xuemin Zhao (xueminzhao@tencent.com)

#include "app/qzap/text_analysis/topic/base/random.h"

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

namespace qzap {
namespace text_analysis {
namespace base {

/*static*/
uint32_t Random::GetTickCount() {
  struct timeval t;
  gettimeofday(&t, NULL);
  t.tv_sec %= (24 * 60 * 60);  // one day ticks 24*60*60
  uint32_t tick_count = t.tv_sec * 1000 + t.tv_usec / 1000;
  return tick_count;
}

void MTRandom::SeedRNG(int seed) {
  if (seed < 0) {
    seed = GetTickCount();
  }

  const uint32_t factor = 1812433253UL;
  mt[0]= seed;
  for (mti = 1; mti < MERS_N; ++mti) {
    mt[mti] = (factor * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
  }

  // Randomize some more
  for (int i = 0; i < 37; i++) {
    Uint32();
  }
}

uint32_t MTRandom::Uint32() {
  // Generate 32 random bits
  uint32_t y;

  if (mti >= MERS_N) {
    // Generate MERS_N words at one time
    const uint32_t LOWER_MASK = (1LU << MERS_R) - 1;
    const uint32_t UPPER_MASK = 0xFFFFFFFF << MERS_R;
    static const uint32_t mag01[2] = {0, MERS_A};

    uint32_t kk;
    for (kk = 0; kk < MERS_N-MERS_M; ++kk) {
      y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
      mt[kk] = mt[kk+MERS_M] ^ (y >> 1) ^ mag01[y & 1];}

    for (; kk < MERS_N-1; kk++) {
      y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
      mt[kk] = mt[kk+(MERS_M-MERS_N)] ^ (y >> 1) ^ mag01[y & 1];}

    y = (mt[MERS_N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
    mt[MERS_N-1] = mt[MERS_M-1] ^ (y >> 1) ^ mag01[y & 1];
    mti = 0;
  }
  y = mt[mti++];

  // Tempering (May be omitted):
  y ^=  y >> MERS_U;
  y ^= (y << MERS_S) & MERS_B;
  y ^= (y << MERS_T) & MERS_C;
  y ^=  y >> MERS_L;

  return y;
}

double MTRandom::RandDouble() {
  // Output random float number in the interval 0 <= x < 1
  // Multiply by 2^(-32)
  return Uint32() * (1./(65536.*65536.));
}

}  // namespace base
}  // namespace text_analysis
}  // namespace qzap
