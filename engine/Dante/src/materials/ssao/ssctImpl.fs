/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ssct.fs"

float dominantLightShadowing(highp vec2 uv, highp vec3 origin, vec3 normal) {
    ConeTraceSetup cone;

    cone.ssStartPos = uv * materialParams.resolution.xy;
    cone.vsStartPos = origin;
    cone.vsNormal = normal;

    cone.vsConeDirection = materialParams.ssctVsLightDirection;
    cone.shadowDistance = materialParams.ssctShadowDistance;
    cone.coneAngleTangeant = materialParams.ssctConeAngleTangeant;
    cone.contactDistanceMaxInv = materialParams.ssctContactDistanceMaxInv;

    cone.screenFromViewMatrix = materialParams.screenFromViewMatrix;
    cone.projectionScale = materialParams.projectionScale;
    cone.resolution = materialParams.resolution;
    cone.maxLevel = float(materialParams.maxLevel);

    cone.intensity = materialParams.ssctIntensity;
    cone.depthBias = materialParams.ssctDepthBias.x;
    cone.slopeScaledDepthBias = materialParams.ssctDepthBias.y;
    cone.sampleCount = materialParams.ssctSampleCount;

    return ssctDominantLightShadowing(uv, origin, normal,
            materialParams_depth, getFragCoord(materialParams.resolution.xy),
            materialParams.ssctRayCount, cone);
}
