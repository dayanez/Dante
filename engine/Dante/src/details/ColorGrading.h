/*
 * Copyright (C) 2020 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_DETAILS_COLORGRADING_H
#define TNT_FILAMENT_DETAILS_COLORGRADING_H

#include "downcast.h"

#include <filament/ColorGrading.h>

#include <backend/Handle.h>

#include <math/mathfwd.h>

#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif

#include <cstddef>
#include <cstdint>

namespace filament {

class FEngine;

class FColorGrading : public ColorGrading {
public:
    FColorGrading(FEngine& engine, const Builder& builder);
    FColorGrading(const FColorGrading& rhs) = delete;
    FColorGrading& operator=(const FColorGrading& rhs) = delete;

    ~FColorGrading() noexcept;

    // frees driver resources, object becomes invalid
    void terminate(FEngine& engine);

    backend::TextureHandle getHwHandle() const noexcept { return mLutHandle; }
    uint32_t getDimension() const noexcept { return mDimension; }
    bool isOneDimensional() const noexcept { return mIsOneDimensional; }
    bool isLDR() const noexcept { return mIsLDR; }

private:
    struct Config;
    static math::float4 hdrColorAt(Builder const& builder, Config const& config,
            size_t r, size_t g, size_t b) noexcept;

    backend::TextureHandle mLutHandle;
    uint32_t mDimension;
    bool mIsOneDimensional;
    bool mIsLDR;

#if defined(__ARM_NEON)
    static void generateDefaultLUTNeon(FEngine const& engine, void* data, Config const& config, Builder const& builder) noexcept;
    static void generateMediumLUTNeon(FEngine const& engine, void* data, Config const& config, Builder const& builder) noexcept;
    static void colorGradingAdjustmentsNeon(
            float32x4_t& vr, float32x4_t& vg, float32x4_t& vb,
            Config const& config, Builder const& builder) noexcept;
#endif

};

FILAMENT_DOWNCAST(ColorGrading)

} // namespace filament

#endif //TNT_FILAMENT_DETAILS_COLORGRADING_H
