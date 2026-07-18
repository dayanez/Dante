/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "flare.h"
#include "generated/resources/flare.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace filament {

static const StaticMaterialInfo sMaterialList[] = {
        { "flare",                      MATERIAL(FLARE, FLARE) },
};

utils::Slice<const StaticMaterialInfo> getFlareMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace filament
