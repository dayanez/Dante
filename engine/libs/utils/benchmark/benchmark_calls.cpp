
#include "PerformanceCounters.h"

#include <utils/compiler.h>

#include <benchmark/benchmark.h>

// ------------------------------------------------------------------------------------------------
//
// Running benchmarks on Android
//
// adb push out/cmake-android-release-aarch64/libs/utils/benchmark_utils /data/local/tmp
// adb push out/cmake-android-release-aarch64/libs/utils/libbenchmark_utils_callee.so /data/local/tmp
// adb shell LD_LIBRARY_PATH=/data/local/tmp /data/local/tmp/benchmark_utils --benchmark_color=true --benchmark_counters_tabular=true
// ------------------------------------------------------------------------------------------------

#define REPEAT 128

extern void bar() noexcept;

UTILS_NOINLINE
void foo() noexcept {
    benchmark::ClobberMemory();
}

class VirtualizerInterface {
public:
    virtual void baz() noexcept = 0;
    void bazz() noexcept;
};

UTILS_NOINLINE
void VirtualizerInterface::bazz() noexcept {
    baz();
}

class Virtualizer : public VirtualizerInterface {
public:
    void baz() noexcept override;
};


UTILS_NOINLINE
void Virtualizer::baz() noexcept {
    benchmark::ClobberMemory();
}

static void BM_call_local(benchmark::State& state) {
    PerformanceCounters pc(state);
#pragma unroll(REPEAT)
    for (auto _ UTILS_UNUSED: state) {
        foo();
    }
}

static void BM_call_library(benchmark::State& state) {
    PerformanceCounters pc(state);
#pragma unroll(REPEAT)
    for (auto _ UTILS_UNUSED: state) {
        bar();
    }
}

static void BM_call_virtual(benchmark::State& state) {
    Virtualizer base;
    VirtualizerInterface* v = &base;

    benchmark::ClobberMemory();
    benchmark::DoNotOptimize(base);
    benchmark::DoNotOptimize(v);

    PerformanceCounters pc(state);
#pragma unroll(REPEAT)
    for (auto _ UTILS_UNUSED: state) {
        v->baz();
    }
}

static void BM_call_virtual_no_unroll(benchmark::State& state) {
    Virtualizer base;
    VirtualizerInterface* v = &base;

    benchmark::ClobberMemory();
    benchmark::DoNotOptimize(base);
    benchmark::DoNotOptimize(v);

    PerformanceCounters pc(state);
#pragma unroll(8)
    for (auto _ UTILS_UNUSED: state) {
        v->baz();
    }
}


static void BM_call_virtual_trampoline(benchmark::State& state) {
    Virtualizer base;
    VirtualizerInterface* v = &base;

    benchmark::ClobberMemory();
    benchmark::DoNotOptimize(base);
    benchmark::DoNotOptimize(v);

    PerformanceCounters pc(state);
#pragma unroll(REPEAT)
    for (auto _ UTILS_UNUSED: state) {
        v->bazz();
    }
}

static void BM_call_function_ptr(benchmark::State& state) {
    using PFN = void(*)();
    PFN pfn = foo;
    benchmark::DoNotOptimize(pfn);
    PerformanceCounters pc(state);
#pragma unroll(REPEAT)
    for (auto _ UTILS_UNUSED: state) {
        pfn();
    }
}

BENCHMARK(BM_call_local);
BENCHMARK(BM_call_library);
BENCHMARK(BM_call_virtual);
BENCHMARK(BM_call_virtual_no_unroll);
BENCHMARK(BM_call_virtual_trampoline);
BENCHMARK(BM_call_function_ptr);
