
#ifndef TNT_FILAMAT_MATERIALINFO_H
#define TNT_FILAMAT_MATERIALINFO_H

#include <private/dante/BufferInterfaceBlock.h>
#include <private/dante/SamplerInterfaceBlock.h>
#include <private/dante/SubpassInfo.h>

#include <dante/MaterialEnums.h>

#include <backend/DriverEnums.h>

#include <utils/compiler.h>
#include <utils/FixedCapacityVector.h>

namespace filamat {

using UniformType = dante::backend::UniformType;
using SamplerType = dante::backend::SamplerType;
using CullingMode = dante::backend::CullingMode;

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
    dante::SpecularAmbientOcclusion specularAO;
    dante::RefractionMode refractionMode;
    dante::RefractionType refractionType;
    dante::ReflectionMode reflectionMode;
    dante::AttributeBitset requiredAttributes;
    dante::BlendingMode blendingMode;
    dante::BlendingMode postLightingBlendingMode;
    dante::Shading shading;
    dante::BufferInterfaceBlock uib;
    dante::SamplerInterfaceBlock sib;
    dante::SubpassInfo subpass;
    dante::ShaderQuality quality;
    dante::backend::FeatureLevel featureLevel;
    dante::backend::StereoscopicType stereoscopicType;
    dante::math::uint3 groupSize;

    using BufferContainer = utils::FixedCapacityVector<dante::BufferInterfaceBlock const*>;
    BufferContainer buffers{ BufferContainer::with_capacity(dante::backend::MAX_SSBO_COUNT) };
};

}
#endif // TNT_FILAMAT_MATERIALINFO_H
