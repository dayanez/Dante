/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_DESCRIPTORSETS_H
#define TNT_FILAMENT_DESCRIPTORSETS_H

#include <private/filament/EngineEnums.h>
#include <private/filament/Variant.h>

#include <filament/MaterialEnums.h>

#include <backend/DriverEnums.h>

#include <utils/CString.h>

namespace filament::descriptor_sets {

backend::DescriptorSetLayout const& getDepthVariantLayout() noexcept;
backend::DescriptorSetLayout const& getSsrVariantLayout() noexcept;

backend::DescriptorSetLayout const& getPerRenderableLayout() noexcept;

backend::DescriptorSetLayout getPerViewDescriptorSetLayout(
        MaterialDomain domain,
        bool isLit, bool isSSR, bool hasFog,
        bool isShadowSampler2D) noexcept;

backend::DescriptorSetLayout getPerViewDescriptorSetLayoutWithVariant(
        Variant variant,
        MaterialDomain domain,
        bool isLit, bool isSSR, bool hasFog) noexcept;

utils::CString getDescriptorName(
        DescriptorSetBindingPoints set,
        backend::descriptor_binding_t binding) noexcept;

backend::DescriptorType getDescriptorType(backend::SamplerType type, backend::SamplerFormat format);

} // namespace filament::descriptor_sets


#endif //TNT_FILAMENT_DESCRIPTORSETS_H
