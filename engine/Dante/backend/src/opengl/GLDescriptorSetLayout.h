/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_OPENGL_GLDESCRIPTORSETLAYOUT_H
#define TNT_FILAMENT_BACKEND_OPENGL_GLDESCRIPTORSETLAYOUT_H

#include "DriverBase.h"

#include <backend/DriverEnums.h>

#include <algorithm>
#include <utility>

#include <stdint.h>

namespace filament::backend {

struct GLDescriptorSetLayout : public HwDescriptorSetLayout, public DescriptorSetLayout {
    using HwDescriptorSetLayout::HwDescriptorSetLayout;
    explicit GLDescriptorSetLayout(DescriptorSetLayout&& layout) noexcept
            : DescriptorSetLayout(std::move(layout)) {

        std::sort(descriptors.begin(), descriptors.end(),
                [](auto&& lhs, auto&& rhs){
            return lhs.binding < rhs.binding;
        });

        auto p = std::max_element(descriptors.cbegin(), descriptors.cend(),
                [](auto const& lhs, auto const& rhs) {
            return lhs.binding < rhs.binding;
        });
        maxDescriptorBinding = p->binding;
    }
    uint8_t maxDescriptorBinding = 0;
};

} // namespace filament::backend

#endif //TNT_FILAMENT_BACKEND_OPENGL_GLDESCRIPTORSETLAYOUT_H
