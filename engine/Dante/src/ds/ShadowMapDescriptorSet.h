/*
 * Copyright (C) 2022 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_SHADOWMAPDESCRIPTORSET_H
#define TNT_FILAMENT_SHADOWMAPDESCRIPTORSET_H

#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"

#include <private/filament/UibStructs.h>

#include <backend/DriverApiForward.h>
#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <math/vec4.h>

#include <array>

namespace filament {

struct CameraInfo;

class FEngine;
class LightManager;

/*
 * PerShadowMapUniforms manages the UBO needed to generate our shadow maps. Internally it just
 * holds onto a `PerViewUniform` UBO handle, but doesn't keep any shadow copy of it, instead it
 * writes the data directly into the CommandStream, for this reason partial update of the data
 * is not possible.
 */
class ShadowMapDescriptorSet {

public:
    class Transaction {
        friend ShadowMapDescriptorSet;
        PerViewUib* uniforms = nullptr;
        Transaction() = default; // disallow creation by the caller
    };

    explicit ShadowMapDescriptorSet(FEngine& engine) noexcept;

    void terminate(backend::DriverApi& driver);

    // All UBO values that can affect user code must be set here

    static void prepareCamera(Transaction const& transaction,
            FEngine const& engine, const CameraInfo& camera) noexcept;

    static void prepareLodBias(Transaction const& transaction,
            float bias) noexcept;

    static void prepareViewport(Transaction const& transaction,
            backend::Viewport const& physicalViewport, backend::Viewport const& logicalViewport) noexcept;

    static void prepareTime(Transaction const& transaction,
            FEngine const& engine, math::float4 const& userTime) noexcept;

    static void prepareMaterialGlobals(Transaction const& transaction,
            std::array<math::float4, 4> const& materialGlobals) noexcept;

    static void prepareShadowMapping(Transaction const& transaction,
            float vsmExponent, float vsmMaxMoment) noexcept;

    static Transaction open(backend::DriverApi& driver) noexcept;

    // update local data into GPU UBO
    void commit(Transaction& transaction, FEngine const& engine, backend::DriverApi& driver) noexcept;

    // bind this UBO
    void bind(backend::DriverApi& driver) noexcept;

private:
    static PerViewUib& edit(Transaction const& transaction) noexcept;
    backend::Handle<backend::HwBufferObject> mUniformBufferHandle;
    DescriptorSet mDescriptorSet;
};

} // namespace filament

#endif //TNT_FILAMENT_SHADOWMAPDESCRIPTORSET_H
