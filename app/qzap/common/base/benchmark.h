// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
// Support for embedding code to be benchmarked into a unittest
// (or a regtest, or any other program for that matter).
//
// Example usage;

/*

// Define a function that executes the code to be measured a
// specified number of times:
static void BM_StringCreation(int n) {
  while (n-- > 0) {
    string empty_string;
  }
}

// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// Define another benchmark
static void BM_StringCopy(int n) {
  string x = "hello";
  while (n-- > 0) {
    string copy(x);
  }
}
BENCHMARK(BM_StringCopy);

// Augment the main() program to invoke benchmarks if specified
// via the --benchmarks command line flag.  E.g.,
//       my_unittest --benchmarks=all
//       my_unittest --benchmarks=BM_StringCreation
//       my_unittest --benchmarks=String
//       my_unittest --benchmarks='Copy|Creation'
int main(int argc, char** argv) {
  InitGoogle(argv[0], &argc, &argv, true);

  RunSpecifiedBenchmarks(); // Does nothing unless --benchmarks is specified

  ... normal unittest code ...;
}

// Sometimes a family of microbenchmarks can be implemented with
// just one routine that takes an extra argument to specify which
// one of the family of benchmarks to run.  For example, the following
// code defines a family of microbenchmarks for measuring the speed
// of memcpy() calls of different lengths:

static void BM_memcpy(int iters, int nbytes) {
  char* src = new char[n]; char* dst = new char[n];
  memset(src, 'x', n);
  for (int i = 0; i < iters; i++) {
    memcpy(dst, src, n);
  }
  SetBenchmarkBytesProcessed(int64(iters) * int64(n));
  delete[] src; delete[] dst;
}
BENCHMARK_WITH_ARG(BM_memcpy, 8);
BENCHMARK_WITH_ARG(BM_memcpy, 64);
BENCHMARK_WITH_ARG(BM_memcpy, 512);
BENCHMARK_WITH_ARG(BM_memcpy, 1<<10);
BENCHMARK_WITH_ARG(BM_memcpy, 8<<10);
BENCHMARK_WITH_ARG(BM_memcpy, 64<<10);
BENCHMARK_WITH_ARG(BM_memcpy, 512<<10);
BENCHMARK_WITH_ARG(BM_memcpy, 1<<20);

// The preceding code is quite repetitve, and can be replaced with the
// following short-hand.  The following macro will pick a few
// appropriate arguments in the specified range and will generate a
// microbenchmark for each such argument.
BENCHMARK_RANGE(BM_memcpy, 8, 1<<20);

*/

#ifndef APP_QZAP_COMMON_BASE_BENCHMARK_H_
#define APP_QZAP_COMMON_BASE_BENCHMARK_H_

#include <stdint.h>
#include <stdio.h>
#include <string>
// If the --benchmarks flag is empty, do nothing.
//
// Otherwise, run all benchmarks specified by the --benchmarks flag,
// and exit after running the benchmarks.
extern void RunSpecifiedBenchmarks();

// ------------------------------------------------------
// Routines that can be called from within a benchmark

// Set the number of bytes processed by the current benchmark
// execution.  This routine is typically called once at the end of a
// throughput oriented benchmark.  If this routine is called with a
// value > 0, the report is printed in MB/sec instead of nanoseconds
// per iteration.
//
// REQUIRES: a benchmark is currently executing
extern void SetBenchmarkBytesProcessed(int64_t bytes);

// If this routine is called with items > 0, then an items/s
// label is printed on the benchmark report line for the currently
// executing benchmark. It is typically called at the end of a processing
// benchmark where a processing items/second output is desired.
//
// REQUIRED: a benchmark is currently executing
extern void SetBenchmarkItemsProcessed(int64_t items);

// If this routine is called, the specified label is printed on the
// benchmark report line for the currently executing benchmark.
//
// REQUIRES: a benchmark is currently executing
extern void SetBenchmarkLabel(const std::string& label);

extern void StartBenchmarkTiming();
extern void StopBenchmarkTiming();

// ------------------------------------------------------
// Macros to register benchmarks

// Invoke this macro at the top-level of the file and pass it the
// name of a routine with the following signature:
//      void (*function)(int n);
// The corresponding function will be invoked as the benchmark.
// It should execute the code to be measured "n" times.
#define BENCHMARK(n) static BenchmarkRegisterer __benchmark_ ## n (#n, &n)

// Ugly helpers needed to get around preprocessor wackiness
#define BENCHMARK_CONCAT(a, b, c) BENCHMARK_CONCAT2(a, b, c)
#define BENCHMARK_CONCAT2(a, b, c) a ## _ ## b ## _ ## c

#define BENCHMARK_WITH_ARG(n, a) \
static BenchmarkRegisterer BENCHMARK_CONCAT(__benchmark_, n, __LINE__)(#n, &n, (a))  // NOLINT

// This will generate an appropriate set of benchmarks based on
// a progression between lo and hi by multiples of 8.  It always
// includes lo and hi, and is roughly equivalent to the sequence:
// BENCHMARK_WITH_ARGS(n, lo)
// BENCHMARK_WITH_ARGS(n, ...)
// BENCHMARK_WITH_ARGS(n, hi)
//
// REQUIRES: "lo >= 0"
// REQUIRES: "hi >= lo"
#define BENCHMARK_RANGE(n, lo, hi) \
static BenchmarkRegisterer __benchmark_ ## n(#n, &n, (lo), (hi))

// ------------------------------------------------------
// Internal implementation details follow; please ignore

class BenchmarkRegisterer {
 public:
  BenchmarkRegisterer(const char* name, void (*function)(int iters));
  BenchmarkRegisterer(const char* name,
                      void (*function)(int iters, int arg),
                      int arg);
  BenchmarkRegisterer(const char* name,
                      void (*function)(int iters, int arg),
                      int lo, int hi);
};

#endif  // APP_QZAP_COMMON_BASE_BENCHMARK_H_
