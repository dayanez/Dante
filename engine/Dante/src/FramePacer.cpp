/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/FramePacer.h"

#include "FilamentAPI-impl.h"

#include "details/Engine.h"
#include "details/Renderer.h"

#include <filament/FramePacer.h>

#include <utils/Panic.h>

namespace filament {

// ------------------------------------------------------------------------------------------------
// FramePacer::Builder
// ------------------------------------------------------------------------------------------------

using BuilderType = FramePacer;
BuilderType::Builder::Builder() noexcept = default;
BuilderType::Builder::~Builder() noexcept = default;
BuilderType::Builder::Builder(Builder const& rhs) noexcept = default;
BuilderType::Builder::Builder(Builder&& rhs) noexcept = default;
BuilderType::Builder& BuilderType::Builder::operator=(Builder const& rhs) noexcept = default;
BuilderType::Builder& BuilderType::Builder::operator=(Builder&& rhs) noexcept = default;

FramePacer::Builder& FramePacer::Builder::targetFrameRate(float const fps) noexcept {
    mImpl->mConfig.targetFrameRate = fps;
    return *this;
}

FramePacer::Builder& FramePacer::Builder::latency(std::chrono::nanoseconds const latency) noexcept {
    mImpl->mConfig.latency = latency;
    return *this;
}

FramePacer::Builder& FramePacer::Builder::latencyFrames(uint32_t const frames) noexcept {
    double const seconds = static_cast<double>(frames) / 60.0;
    return latency(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::duration<double>(seconds)));
}

FramePacer* FramePacer::Builder::build(Engine& engine) const {
    FILAMENT_CHECK_PRECONDITION(mImpl->mConfig.targetFrameRate >= 0.0f)
            << "targetFrameRate must be non-negative";
    FILAMENT_CHECK_PRECONDITION(mImpl->mConfig.latency > std::chrono::nanoseconds::zero())
            << "latency must be greater than 0";

    return downcast(engine).createFramePacer(*this);
}

// ------------------------------------------------------------------------------------------------
// FramePacer Trampoline Methods
// ------------------------------------------------------------------------------------------------

void FramePacer::configure(const Configuration& config) {
    downcast(this)->configure(config);
}

const FramePacer::Configuration& FramePacer::getConfiguration() const noexcept {
    return downcast(this)->getConfiguration();
}

FramePacer::FrameStatus FramePacer::setupFrame(const VsyncTick& tick) {
    return downcast(this)->setupFrame(tick);
}

bool FramePacer::setupExtraFrame() noexcept {
    return downcast(this)->setupExtraFrame();
}

void FramePacer::applyPresentationTime(Renderer* renderer) {
    downcast(this)->applyPresentationTime(downcast(renderer));
}

bool FramePacer::hasGpuFallenBehind(Renderer* renderer) {
    return downcast(this)->hasGpuFallenBehind(downcast(renderer));
}

FramePacer::time_point_t FramePacer::getExpectedPresentationTime() const noexcept {
    return downcast(this)->getExpectedPresentationTime();
}

FramePacer::time_point_t FramePacer::getRenderingDeadline() const noexcept {
    return downcast(this)->getRenderingDeadline();
}

std::chrono::nanoseconds FramePacer::getEffectiveLatency() const noexcept {
    return downcast(this)->getEffectiveLatency();
}

float FramePacer::getSelectedFrameRate() const noexcept {
    return downcast(this)->getSelectedFrameRate();
}

bool FramePacer::isExactFrameRateAchieved() const noexcept {
    return downcast(this)->isExactFrameRateAchieved();
}

FramePacer::PacingStatus FramePacer::getPacingStatus() const noexcept {
    return downcast(this)->getPacingStatus();
}

void FramePacer::resetPacing() noexcept {
    downcast(this)->resetPacing();
}

} // namespace filament
