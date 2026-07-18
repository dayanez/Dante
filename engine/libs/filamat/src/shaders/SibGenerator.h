/*
 * Copyright (C) 2015 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILABRIDGE_SIBGENERATOR_H
#define TNT_FILABRIDGE_SIBGENERATOR_H

#include <private/filament/EngineEnums.h>
#include <private/filament/Variant.h>

#include <stddef.h>
#include <stdint.h>

namespace filament {

class SamplerInterfaceBlock;

class SibGenerator {
public:
    static SamplerInterfaceBlock const& getPerViewSib(Variant variant) noexcept;
    static SamplerInterfaceBlock const& getPerRenderableSib(Variant variant) noexcept;
    static SamplerInterfaceBlock const* getSib(filament::DescriptorSetBindingPoints bindingPoint, Variant variant) noexcept;
    // When adding a sampler block here, make sure to also update
    //      FMaterial::getSurfaceProgramSlow and FMaterial::getPostProcessProgramSlow if needed
};

} // namespace filament

#endif // TNT_FILABRIDGE_SIBGENERATOR_H
