/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "PostProcessDescriptorSet.h"

#include "HwDescriptorSetLayoutFactory.h"
#include "TypedUniformBuffer.h"

#include "details/Engine.h"

#include <private/filament/DescriptorSets.h>
#include <private/filament/EngineEnums.h>
#include <private/filament/UibStructs.h>

#include <backend/DriverEnums.h>

namespace filament {

using namespace backend;
using namespace math;

PostProcessDescriptorSet::PostProcessDescriptorSet() noexcept = default;

void PostProcessDescriptorSet::init(FEngine& engine) noexcept {

    // create the descriptor-set layout
    mDescriptorSetLayout = filament::DescriptorSetLayout{
            engine.getDescriptorSetLayoutFactory(),
            engine.getDriverApi(), descriptor_sets::getDepthVariantLayout() };

    // create the descriptor-set from the layout
    mDescriptorSet = DescriptorSet{ "PostProcessDescriptorSet", mDescriptorSetLayout };
}

void PostProcessDescriptorSet::terminate(HwDescriptorSetLayoutFactory& factory, DriverApi& driver) {
    mDescriptorSet.terminate(driver);
    mDescriptorSetLayout.terminate(factory, driver);
}

void PostProcessDescriptorSet::setFrameUniforms(DriverApi& driver,
        TypedUniformBuffer<PerViewUib>& uniforms) noexcept {
    // initialize the descriptor-set
    mDescriptorSet.setBuffer(mDescriptorSetLayout,
            +PerViewBindingPoints::FRAME_UNIFORMS,
            uniforms.getUboHandle(), 0, uniforms.getSize());

    mDescriptorSet.commit(mDescriptorSetLayout, driver);
}

void PostProcessDescriptorSet::bind(DriverApi& driver) noexcept {
    mDescriptorSet.bind(driver, DescriptorSetBindingPoints::PER_VIEW);
}

} // namespace filament

