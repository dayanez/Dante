/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FrameSkipper.h"

#include <backend/DriverEnums.h>

#include <utils/compiler.h>
#include <utils/debug.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <utility>

#include <stddef.h>

namespace filament {

using namespace utils;
using namespace backend;

FrameSkipper::FrameSkipper(size_t const latency) noexcept
        : mLatency(std::clamp(latency, size_t(1), MAX_FRAME_LATENCY) - 1) {
}

FrameSkipper::~FrameSkipper() noexcept = default;

void FrameSkipper::terminate(DriverApi& driver) noexcept {
    for (auto& fence : mDelayedFences) {
        if (fence) {
            driver.destroyFence(std::move(fence));
        }
    }
}

bool FrameSkipper::shouldRenderFrame(DriverApi& driver) const noexcept {

    if (UTILS_UNLIKELY(mFrameToSkip)) {
        return false;
    }

    auto& fences = mDelayedFences;
    if (fences.front()) {
        // Do we have a latency old fence?
        FenceStatus const status = driver.getFenceStatus(fences.front());
        if (UTILS_UNLIKELY(status == FenceStatus::TIMEOUT_EXPIRED)) {
            // The fence hasn't signaled yet, skip this frame
            return false;
        }
        // If we get a FenceStatus::ERROR, it doesn't necessarily indicate a "bug", it could
        // just be that fences are not supported. Regardless, we should return `true` in that
        // case.
        assert_invariant(status != FenceStatus::TIMEOUT_EXPIRED);
    }
    return true;
}

void FrameSkipper::submitFrame(DriverApi& driver) noexcept {
    auto& fences = mDelayedFences;
    size_t const last = mLatency;

    // pop the oldest fence and advance the other ones
    if (fences.front()) {
        driver.destroyFence(fences.front());
    }
    std::move(fences.begin() + 1, fences.end(), fences.begin());

    // add a new fence to the end
    assert_invariant(!fences[last]);

    fences[last] = driver.createFence();
}

void FrameSkipper::skipNextFrames(size_t frameCount) noexcept {
    frameCount = std::min(frameCount, size_t(std::numeric_limits<decltype(mFrameToSkip)>::max()));
    mFrameToSkip = uint16_t(frameCount);
}

size_t FrameSkipper::getFrameToSkipCount() const noexcept {
    return mFrameToSkip;
}


} // namespace filament
