// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5
// $build64_release/app/qzap/common/utility/hash_test --benchmarks=all
// Run on (2 X 2133 MHz CPUs); 2013/03/04-21:52:09.322
// Benchmark        Time(ns)    CPU(ns) Iterations
// -----------------------------------------------
// BM_HashCompare    1006325    1003823     100000
// BM_Compare        1071296    1063186     100000

#include <math.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include "app/qzap/common/base/benchmark.h"
#include "app/qzap/common/utility/hash.h"
#include "app/qzap/common/base/shared_ptr.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
class HashTest : public testing::Test {
};

TEST_F(HashTest, Test1) {
  std::string full = "hello";
  std::string part1 = "he";
  std::string part2 = "llo";
  uint64_t f1 = hash_data(
    reinterpret_cast<const uint8_t *>(full.c_str()),
    full.size(), 0xbeef);
  ASSERT_EQ(f1, hash_string(full));
}

static std::vector<std::string> a;
static std::vector<std::string> b;
static std::vector<uint64_t> bf;

DECLARE_string(benchmarks);
int main(int argc, char **argv) {
  FLAGS_logtostderr = true;
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  for (int i = 0; i < 5; i++) {
    std::string c = "1234567890";
    std::string r, h;
    for (int k = 0; k < 1024*100; k++) {
      r += c;
      h += c;
    }
    a.push_back(r);
    b.push_back(h);
    bf.push_back(hash_string(r));
  }

  RunSpecifiedBenchmarks();
  return RUN_ALL_TESTS();
}

static void BM_HashCompare(int iters) {
  for (int i = 0; i < iters; i++) {
    int32_t j = i % a.size();
    CHECK(hash_string(a[j]) == bf[j]);
  }
}
BENCHMARK(BM_HashCompare);

static void BM_Compare(int iters) {
  for (int i = 0; i < iters; i++) {
    int32_t j = i % a.size();
    CHECK(a[j] == b[j]);
  }
}
BENCHMARK(BM_Compare);
