/*
 * Copyright (C) 2018 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "PerformanceCounters.h"

#include <utils/compiler.h>

#include <benchmark/benchmark.h>

#include <random>
#include <vector>

using namespace utils;


class BinarySearch : public benchmark::Fixture {
public:
     BinarySearch();
    ~BinarySearch() override;

protected:
    using value_type = uint64_t;
    std::vector<value_type> data;
    std::default_random_engine gen{123};
    std::uniform_int_distribution<value_type> nd;

    std::vector<value_type> prepareItems(size_t size);
};

BinarySearch::BinarySearch() {
    data.resize(2<<20);
    std::generate(data.begin(), data.end(), [&](){ return nd(gen); });
    std::sort(data.begin(), data.end());
}

BinarySearch::~BinarySearch() = default;

UTILS_NOINLINE
std::vector<BinarySearch::value_type> BinarySearch::prepareItems(size_t size) {
    auto first = data.begin();
    auto last = data.begin() + size;
    std::vector<value_type> indices(last - first);
    std::copy(first, last, indices.begin());
    std::default_random_engine gen{123};
    std::shuffle(indices.begin(), indices.end(), gen);
    return indices;
}

BENCHMARK_DEFINE_F(BinarySearch, linearSearch)(benchmark::State& state) {
    auto first = data.begin();
    auto last = data.begin() + state.range(0);
    std::vector<value_type> indices = prepareItems(state.range(0));
    value_type const* ip = indices.data();
    size_t i = 0;

    {
        PerformanceCounters pc(state);
        for (auto _ UTILS_UNUSED: state) {
            auto item = ip[i++ % state.range(0)];
            auto const& pos = std::find(first, last, item);
            benchmark::DoNotOptimize(pos);
        }
    }
}

BENCHMARK_DEFINE_F(BinarySearch, stdLowerBound)(benchmark::State& state) {
    auto first = data.begin();
    auto last = data.begin() + state.range(0);
    std::vector<value_type> indices = prepareItems(state.range(0));
    value_type const* ip = indices.data();
    size_t i = 0;

    {
        PerformanceCounters pc(state);
        for (auto _ UTILS_UNUSED: state) {
            auto item = ip[i++ % state.range(0)];
            auto const& pos = std::lower_bound(first, last, item);
            benchmark::DoNotOptimize(pos);
        }
    }
}

BENCHMARK_REGISTER_F(BinarySearch, linearSearch)->Range(2, 1<<20);
BENCHMARK_REGISTER_F(BinarySearch, stdLowerBound)->Range(2, 1<<20);
