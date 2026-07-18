/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_POSTPROCESSINGDESCRIPTORSET_H
#define TNT_FILAMENT_POSTPROCESSINGDESCRIPTORSET_H

#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "TypedUniformBuffer.h"

#include <private/filament/UibStructs.h>

#include <backend/DriverApiForward.h>

namespace filament {

class FEngine;
class HwDescriptorSetLayoutFactory;

class PostProcessDescriptorSet {
public:
    explicit PostProcessDescriptorSet() noexcept;

    void init(FEngine& engine) noexcept;

    void terminate(HwDescriptorSetLayoutFactory& factory, backend::DriverApi& driver);

    void setFrameUniforms(backend::DriverApi& driver,
            TypedUniformBuffer<PerViewUib>& uniforms) noexcept;

    void bind(backend::DriverApi& driver) noexcept;

    DescriptorSetLayout const& getLayout() const noexcept {
        return mDescriptorSetLayout;
    }

private:
    DescriptorSetLayout mDescriptorSetLayout;
    DescriptorSet mDescriptorSet;
};

} // namespace filament

#endif //TNT_FILAMENT_POSTPROCESSINGDESCRIPTORSET_H
