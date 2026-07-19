
#pragma once

#include <private/dante/UibStructs.h>

#include <math/vec2.h>
#include <math/vec4.h>

#include <array>

namespace dante {
namespace backend {
struct Viewport;
} // namespace backend

class FEngine;
struct CameraInfo;

class PerViewDescriptorSetUtils {
public:
    static void prepareCamera(PerViewUib& uniforms,
            FEngine const& engine, const CameraInfo& camera) noexcept;

    static void prepareLodBias(PerViewUib& uniforms,
            float bias, math::float2 derivativesScale) noexcept;

    static void prepareViewport(PerViewUib& uniforms,
            backend::Viewport const& physicalViewport,
            backend::Viewport const& logicalViewport) noexcept;

    static void prepareTime(PerViewUib& uniforms,
            FEngine const& engine, math::float4 const& userTime) noexcept;

    static void prepareMaterialGlobals(PerViewUib& uniforms,
            std::array<math::float4, 4> const& materialGlobals) noexcept;
};

} //namespace dante
