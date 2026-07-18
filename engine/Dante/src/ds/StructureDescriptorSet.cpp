/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "StructureDescriptorSet.h"

#include "PerViewDescriptorSetUtils.h"

#include "details/Camera.h"
#include "details/Engine.h"

#include <private/filament/EngineEnums.h>
#include <private/filament/UibStructs.h>

#include <utils/compiler.h>
#include <utils/debug.h>

#include <math/vec2.h>
#include <math/vec4.h>

#include <array>

namespace filament {

using namespace backend;
using namespace math;

StructureDescriptorSet::StructureDescriptorSet() noexcept = default;

StructureDescriptorSet::~StructureDescriptorSet() noexcept {
    assert_invariant(!mDescriptorSet.getHandle());
}

void StructureDescriptorSet::init(FEngine& engine) noexcept {

    mUniforms.init(engine.getDriverApi());

    mDescriptorSetLayout = &engine.getPerViewDescriptorSetLayoutDepthVariant();

    // create the descriptor-set from the layout
    mDescriptorSet = DescriptorSet{
        "StructureDescriptorSet", *mDescriptorSetLayout };

    // initialize the descriptor-set
    mDescriptorSet.setBuffer(*mDescriptorSetLayout, +PerViewBindingPoints::FRAME_UNIFORMS,
            mUniforms.getUboHandle(), 0, sizeof(PerViewUib));
}

void StructureDescriptorSet::terminate(DriverApi& driver) {
    mDescriptorSet.terminate(driver);
    mUniforms.terminate(driver);
}

void StructureDescriptorSet::bind(DriverApi& driver) const noexcept {
    assert_invariant(mDescriptorSetLayout);
    if (mUniforms.isDirty()) {
        mUniforms.clean();
        driver.updateBufferObject(mUniforms.getUboHandle(),
                mUniforms.toBufferDescriptor(driver), 0);
        mDescriptorSet.commit(*mDescriptorSetLayout, driver);
    }
    mDescriptorSet.bind(driver, DescriptorSetBindingPoints::PER_VIEW);
}

void StructureDescriptorSet::prepareViewport(
        backend::Viewport const& physicalViewport,
        backend::Viewport const& logicalViewport) noexcept {
    PerViewDescriptorSetUtils::prepareViewport(mUniforms.edit(), physicalViewport, logicalViewport);
}

void StructureDescriptorSet::prepareCamera(FEngine const& engine,
        CameraInfo const& camera) noexcept {
    PerViewDescriptorSetUtils::prepareCamera(mUniforms.edit(), engine, camera);
}

void StructureDescriptorSet::prepareLodBias(float bias, float2 const derivativesScale) noexcept {
    PerViewDescriptorSetUtils::prepareLodBias(mUniforms.edit(), bias, derivativesScale);
}

void StructureDescriptorSet::prepareTime(FEngine const& engine, float4 const& userTime) noexcept {
    PerViewDescriptorSetUtils::prepareTime(mUniforms.edit(), engine, userTime);
}

void StructureDescriptorSet::prepareMaterialGlobals(
        std::array<float4, 4> const& materialGlobals) noexcept {
    PerViewDescriptorSetUtils::prepareMaterialGlobals(mUniforms.edit(), materialGlobals);
}

} // namespace filament
