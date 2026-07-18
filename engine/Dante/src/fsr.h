/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_FSR_H
#define TNT_FILAMENT_FSR_H

#include <filament/Viewport.h>

#include <math/vec4.h>

#include <stdint.h>

namespace filament {

struct FSRScalingConfig {
    backend::Backend backend;
    Viewport input;   // region of source to be scaled
    uint32_t inputWidth;        // width of source
    uint32_t inputHeight;       // height of source
    uint32_t outputWidth;       // width of destination
    uint32_t outputHeight;      // height of destination
};

struct FSRSharpeningConfig {
    // The scale is {0.0 := maximum sharpness, to N>0, where N is the number of stops (halving)
    // of the reduction of sharpness}.
    float sharpness;
};

struct FSRUniforms {
    math::float4 EasuCon0;
    math::float4 EasuCon1;
    math::float4 EasuCon2;
    math::float4 EasuCon3;
    math::uint4 RcasCon;
};

void FSR_ScalingSetup(FSRUniforms* inoutUniforms, FSRScalingConfig config) noexcept;
void FSR_SharpeningSetup(FSRUniforms* inoutUniforms, FSRSharpeningConfig config) noexcept;

} // namespace filament

#endif // TNT_FILAMENT_FSR_H
