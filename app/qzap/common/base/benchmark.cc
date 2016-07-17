// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
#include <regex.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <algorithm>
#include <limits>
#include <string>
#include <vector>
#include "app/qzap/common/base/benchmark.h"
#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/walltime.h"
#include "app/qzap/common/thread/mutex.h"
#include "app/qzap/common/base/sysinfo.h"

DEFINE_string(benchmarks, "",
              "A regular expression that specifies the set of benchmarks "
              "to execute.  If this flag is empty, no benchmarks are run.  "
              "If this flag is the std::string \"all\", all benchmarks linked "
              "into the process are run.");

DEFINE_int32(benchmark_min_iters, 100000,
             "Minimum number of iterations per benchmark");

DEFINE_int32(benchmark_max_iters, 2000000000,
             "Maximum number of iterations per benchmark");

DEFINE_double(benchmark_min_time, 0.5,
              "Minimum number of seconds we should run benchmark before "
              "results are considered significant");

// Information kept per benchmark
namespace {

typedef void (*BenchmarkFunction)(int iterations);
typedef void (*BenchmarkFunctionWithArg)(int iterations, int arg);

struct Benchmark {
  std::string                        name;
  BenchmarkFunction             function;
  BenchmarkFunctionWithArg      function_with_arg;
  int                           arg;

  Benchmark() : function(NULL), function_with_arg(NULL) { }

  void Run(int iters) {
    if (function_with_arg != NULL) {
      (*function_with_arg)(iters, arg);
    } else {
      (*function)(iters);
    }
  }
};

static Mutex benchmark_lock;
static std::vector<Benchmark*>* benchmarks = NULL;

void AddBenchmark(Benchmark* b) {
  MutexLock l(&benchmark_lock);
  if (benchmarks == NULL) {
    benchmarks = new std::vector<Benchmark*>;
  }
  benchmarks->push_back(b);
}

bool running_benchmark = false;

// Global variable for bytes processed by benchmark so far
double bytes_processed = 0;

// Global variable for items processed by benchmark so far
int64_t items_processed = 0;

// Global variable so that a benchmark can cause a little extra printing
char report_label[1024];

// Are timers running?
bool timers_running;

// Starting times if running
double cpu_start_time;
double real_start_time;

// Accumulated benchmark times
double cpu_accumulated_time;
double real_accumulated_time;

// Return prefix to print in front of each reported line
static const char* Prefix() {
#ifdef NDEBUG
    return "";
#else
    return "DEBUG: ";
#endif
}

void RunBenchmark(Benchmark* b, int name_field_width) {
  int iters = FLAGS_benchmark_min_iters;
  while (true) {
    // Try benchmark
    VLOG(1) << "Running " << b->name << " for " << iters;
    bytes_processed = 0;
    items_processed = 0;
    report_label[0] = '\0';
    running_benchmark = true;

    timers_running = false;
    real_accumulated_time = 0;
    cpu_accumulated_time = 0;
    StartBenchmarkTiming();

    b->Run(iters);

    if (timers_running) {
      StopBenchmarkTiming();
    }

    running_benchmark = false;

    VLOG(1) << "Ran in " << cpu_accumulated_time << "/"
            << real_accumulated_time;

    // Was elapsed time or cpu time large enough?
    if ((iters >= FLAGS_benchmark_max_iters) ||
        (cpu_accumulated_time >= FLAGS_benchmark_min_time) ||
        (real_accumulated_time >= 5*FLAGS_benchmark_min_time)) {
      const double seconds = real_accumulated_time;

      // compute MB/s rate
      char rate[100];
      if (bytes_processed > 0) {
        snprintf(rate, sizeof(rate),
                 " %6.1f MB/s",
                 (bytes_processed/1048576.0) / real_accumulated_time);
      } else {
        rate[0] = '\0';
      }

      // compute items/s rate
      char items[100];
      if (items_processed > 0) {
        double value = items_processed / seconds;
        if (value > 10485760) {
          value = value / 1048576;
          snprintf(items, sizeof(items), " %6.1fM items/s", value);
        } else if (items_processed > 10240) {
          value = value / 1024;
          snprintf(items, sizeof(items), " %6.1fK items/s", value);
        } else {
          snprintf(items, sizeof(items), " %6.1f  items/s", value);
        }
      } else {
        items[0] = '\0';
      }

      fprintf(stdout,
              "%s%-*s %10.0f %10.0f %10d%s %s %s\n",
              Prefix(),
              name_field_width,
              b->name.c_str(),
              (real_accumulated_time * 1e9) / (static_cast<double>(iters)),
              (cpu_accumulated_time * 1e9) / (static_cast<double>(iters)),
              iters,
              rate,
              items,
              report_label);
      fflush(stdout);
      return;
    }

    // See how much iterations should be increased by
    double multiplier = 1.4 * FLAGS_benchmark_min_time / cpu_accumulated_time;
    multiplier = std::min(10.0, multiplier);  // At most 10 times expansion
    if (multiplier <= 1.0) multiplier = 2.0;
    double next_iters = multiplier * iters;
    if (next_iters > FLAGS_benchmark_max_iters) {
      next_iters = FLAGS_benchmark_max_iters;
    }
    iters = static_cast<int>(next_iters);
  }
}

}  // namespace

BenchmarkRegisterer::BenchmarkRegisterer(const char* name,
                                         void (*function)(int iters)) {
  Benchmark* benchmark = new Benchmark;
  benchmark->name = name;
  benchmark->function = function;
  AddBenchmark(benchmark);
}

BenchmarkRegisterer::BenchmarkRegisterer(const char* name,
                                         void (*function)(int iters, int arg),
                                         int arg) {
  Benchmark* benchmark = new Benchmark;
  benchmark->function_with_arg = function;
  benchmark->arg = arg;

  // Benchmark name should include the argument
  char suffix[100];
  if ((arg >= 1048576) && ((arg % 1048576) == 0)) {
    // Exact multiple of a MB
    snprintf(suffix, sizeof(suffix), "/%dM", arg/1048576);
  } else if ((arg >= 1024) && ((arg % 1024) == 0)) {
    // Exact multiple of a KB
    snprintf(suffix, sizeof(suffix), "/%dK", arg/1024);
  } else {
    snprintf(suffix, sizeof(suffix), "/%d", arg);
  }
  benchmark->name = name;
  benchmark->name += suffix;

  AddBenchmark(benchmark);
}

BenchmarkRegisterer::BenchmarkRegisterer(const char* name,
                                         void (*function)(int iters, int arg),
                                         int lo,
                                         int hi) {
  CHECK_GE(lo, 0);
  CHECK_GE(hi, lo);

  // Benchmark "lo"
  BenchmarkRegisterer(name, function, lo);

  // Now space out the benchmarks in multiples of 8
  for (int32_t i = 1; i < std::numeric_limits<int32_t>::max()/8; i *= 8) {
    if (i > lo && i < hi) {
      BenchmarkRegisterer(name, function, i);
    }
  }

  // Benchmark "hi" (if different from "lo")
  if (hi != lo) {
    BenchmarkRegisterer(name, function, hi);
  }
}

// Required to avoid triggering a gcc 2.95.3 compiler bug
static void call_exit(int status) { exit(0); }

void RunSpecifiedBenchmarks() {
  std::string spec = FLAGS_benchmarks;
  if (spec.empty()) return;

  if (spec == "all") {
    spec = ".";         // Regexp that matches all benchmarks
  }

  // Make regular expression out of command-line flag
  regex_t re;
  int ec = regcomp(&re, spec.c_str(), REG_EXTENDED | REG_NOSUB);
  if (ec != 0) {
    size_t needed = regerror(ec, &re, NULL, 0);
    char* errbuf = new char[needed];
    regerror(ec, &re, errbuf, needed);
    LOG(FATAL) << "Could not compile benchmark re: " << errbuf;
    delete[] errbuf;
  }

  int num_cpus = System_NumCPUs();
  fprintf(stdout,
          "Run on (%d X %0.0f MHz CPU%s); %s\n",
          num_cpus,
          System_CyclesPerSecond() / 1000000.0f,
          (num_cpus > 1) ? "s" : "",
          WallTime_ToString(WallTime_Now()).c_str());

  {
    MutexLock l(&benchmark_lock);

    // Determine the width of the name field using a minimum width of 10.
    int name_field_width = 10;
    if (benchmarks != NULL) {
      for (size_t i = 0; i < benchmarks->size(); i++) {
        Benchmark* b = (*benchmarks)[i];
        if (regexec(&re, b->name.c_str(), 0, NULL, 0) == 0) {
          name_field_width = std::max<int>(name_field_width, b->name.size());
        }
      }
    }

    int output_width =
        fprintf(stdout,
                "%s%-*s %10s %10s %10s\n",
                Prefix(),
                name_field_width,
                "Benchmark",
                "Time(ns)",
                "CPU(ns)",
                "Iterations");
    fprintf(stdout, "%s\n", std::string(output_width - 1, '-').c_str());

    if (benchmarks != NULL) {
      for (size_t i = 0; i < benchmarks->size(); i++) {
        Benchmark* b = (*benchmarks)[i];
        if (regexec(&re, b->name.c_str(), 0, NULL, 0) == 0) {
          RunBenchmark(b, name_field_width);
        }
      }
    }
  }

  regfree(&re);
  call_exit(0);
}

void SetBenchmarkBytesProcessed(int64_t bytes) {
  CHECK(running_benchmark) << ": SetBenchmarkBytesProcessed() should only "
                           << "be called from within a benchmark";
  bytes_processed = bytes;
}

void SetBenchmarkItemsProcessed(int64_t items) {
  CHECK(running_benchmark) << ": SetBenchmarkItemsProcessed() should only "
                           << "be called from within a benchmark";
  items_processed = items;
}

void SetBenchmarkLabel(const std::string& label) {
  CHECK(running_benchmark) << ": SetBenchmarkLabel() should only "
                           << "be called from within a benchmark";
  strncpy(report_label, label.c_str(), sizeof(report_label));
}

void StopBenchmarkTiming() {
  // Add in time accumulated so far
  CHECK(running_benchmark);
  CHECK(timers_running);
  timers_running = false;
  real_accumulated_time += (WallTime_Now() / 1000.0 - real_start_time);
  cpu_accumulated_time += (MyCPUUsage() - cpu_start_time);
}

void StartBenchmarkTiming() {
  CHECK(running_benchmark);
  CHECK(!timers_running);
  timers_running = true;
  real_start_time = WallTime_Now() / 1000.0;
  cpu_start_time = MyCPUUsage();
}
