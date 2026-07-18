/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_DETAILS_FRAMEPACER_H
#define TNT_FILAMENT_DETAILS_FRAMEPACER_H

#include "downcast.h"

#include <filament/FramePacer.h>

#include <chrono>

namespace filament {

class FEngine;
class FRenderer;

struct FramePacer::BuilderDetails {
    Configuration mConfig;
};

class FFramePacer : public FramePacer {
public:
    FFramePacer(FEngine& engine, Builder const& builder);
    ~FFramePacer();

    void terminate(FEngine& engine) noexcept;

    void configure(Configuration const& config);
    Configuration const& getConfiguration() const noexcept { return mConfig; }
    FrameStatus setupFrame(VsyncTick const& tick);
    bool setupExtraFrame() noexcept;
    bool hasGpuFallenBehind(FRenderer const* renderer);
    void applyPresentationTime(FRenderer* renderer) const;
    time_point_t getExpectedPresentationTime() const noexcept;
    time_point_t getRenderingDeadline() const noexcept;
    std::chrono::nanoseconds getEffectiveLatency() const noexcept;
    PacingStatus getPacingStatus() const noexcept;
    void resetPacing() noexcept;


    float getSelectedFrameRate() const noexcept;
    bool isExactFrameRateAchieved() const noexcept;

private:
    using TargetStepResult = std::pair<duration_t, bool>;
    using HardwareTimelineResult = std::tuple<time_point_t, time_point_t, time_point_t, duration_t>;

    Configuration mConfig;
    duration_t mHardwarePeriod = std::chrono::duration_cast<duration_t>(
            std::chrono::duration<float>(1.0f / 60.0f));
    duration_t mActiveTargetStep = mHardwarePeriod;
    time_point_t mExpectedBaseTime;
    time_point_t mCurrentFrameBaseTime;
    time_point_t mTargetPresentationTime;
    time_point_t mRenderingDeadline;
    time_point_t mAdjustedPresentation;
    time_point_t mLastTargetPresentationTime;
    time_point_t mFrameScheduleTime;
    bool mExactFrameRateAchieved = true;
    bool mConfigLatencyChanged = false;
    PacingStatus mPacingStatus = PacingStatus::STEADY;

    // Returns { targetStep, exactAchieved }
    static TargetStepResult calculateTargetStep(float targetFrameRate, duration_t hardwarePeriod);
    void syncExpectedBaseTimeWithVsync(time_point_t baseTime, duration_t targetStep);
    bool shouldSkipVsync(time_point_t baseTime) const;

    // Returns { candidatePresentation, candidateDeadline, candidateAdjustedPresentation, minIdealDiff }
    static HardwareTimelineResult matchHardwareTimeline(VsyncTick const& tick, time_point_t projectedPresentation,
            time_point_t idealPresentation, duration_t hardwarePeriod);
};
FILAMENT_DOWNCAST(FramePacer)

} // namespace filament

#endif // TNT_FILAMENT_DETAILS_FRAMEPACER_H
