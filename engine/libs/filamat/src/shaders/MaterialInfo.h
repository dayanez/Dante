/*
 * Copyright (C) 2017 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMAT_MATERIALINFO_H
#define TNT_FILAMAT_MATERIALINFO_H

#include <private/filament/BufferInterfaceBlock.h>
#include <private/filament/SamplerInterfaceBlock.h>
#include <private/filament/SubpassInfo.h>

#include <filament/MaterialEnums.h>

#include <backend/DriverEnums.h>

#include <utils/compiler.h>
#include <utils/FixedCapacityVector.h>

namespace filamat {

using UniformType = filament::backend::UniformType;
using SamplerType = filament::backend::SamplerType;
using CullingMode = filament::backend::CullingMode;

struct UTILS_PUBLIC MaterialInfo {
    bool isLit;
    bool hasDoubleSidedCapability;
    bool hasExternalSamplers;
    bool has3dSamplers;
    bool hasShadowMultiplier;
    bool hasTransparentShadow;
    bool specularAntiAliasing;
    bool clearCoatIorChange;
    bool flipUV;
    bool linearFog;
    bool shadowFarAttenuation;
    bool hasColoredPenumbra;
    bool multiBounceAO;
    bool multiBounceAOSet;
    bool specularAOSet;
    bool hasCustomSurfaceShading;
    bool useLegacyMorphing;
    bool instanced;
    bool vertexDomainDeviceJittered;
    bool userMaterialHasCustomDepth;
    int stereoscopicEyeCount;
    filament::SpecularAmbientOcclusion specularAO;
    filament::RefractionMode refractionMode;
    filament::RefractionType refractionType;
    filament::ReflectionMode reflectionMode;
    filament::AttributeBitset requiredAttributes;
    filament::BlendingMode blendingMode;
    filament::BlendingMode postLightingBlendingMode;
    filament::Shading shading;
    filament::BufferInterfaceBlock uib;
    filament::SamplerInterfaceBlock sib;
    filament::SubpassInfo subpass;
    filament::ShaderQuality quality;
    filament::backend::FeatureLevel featureLevel;
    filament::backend::StereoscopicType stereoscopicType;
    filament::math::uint3 groupSize;

    using BufferContainer = utils::FixedCapacityVector<filament::BufferInterfaceBlock const*>;
    BufferContainer buffers{ BufferContainer::with_capacity(filament::backend::MAX_SSBO_COUNT) };
};

}
#endif // TNT_FILAMAT_MATERIALINFO_H
