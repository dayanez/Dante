/*
 * Copyright (C) 2018 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_MATH_BENCHMARK_PEROFRMANCECOUNTERS_H
#define TNT_MATH_BENCHMARK_PEROFRMANCECOUNTERS_H

#include <utils/Profiler.h>

#include <benchmark/benchmark.h>

#include <cmath>

class PerformanceCounters {
    benchmark::State& state;
    utils::Profiler profiler;
    utils::Profiler::Counters counters{};

public:
    explicit PerformanceCounters(benchmark::State& state)
            : state(state) {
        profiler.resetEvents(utils::Profiler::EV_CPU_CYCLES | utils::Profiler::EV_BPU_MISSES);
        profiler.start();
    }

    void stop() {
        profiler.stop();
    }

    ~PerformanceCounters() {
        profiler.stop();
        counters = profiler.readCounters();
        if (profiler.isValid()) {
            auto avgItem = double(state.iterations()) / state.items_processed();
            state.counters.insert({
                    { "C",   { avgItem * (double)counters.getCpuCycles(),    benchmark::Counter::kAvgIterations }},
                    { "I",   { avgItem * (double)counters.getInstructions(), benchmark::Counter::kAvgIterations }},
                    { "BPU", { std::floor(0.5 + avgItem * (double)counters.getBranchMisses() / state.iterations()), benchmark::Counter::kDefaults }},
                    { "CPI", {           (double)counters.getCPI(),          benchmark::Counter::kAvgThreads }},
            });
        }
    }
};

#endif //TNT_MATH_BENCHMARK_PEROFRMANCECOUNTERS_H
