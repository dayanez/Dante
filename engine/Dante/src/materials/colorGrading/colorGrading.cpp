/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "colorGrading.h"
#include "generated/resources/colorGrading.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace filament {

static const StaticMaterialInfo sMaterialList[] = {
    { "colorGrading",               MATERIAL(COLORGRADING, COLORGRADING) },
    { "colorGradingAsSubpass",      MATERIAL(COLORGRADING, COLORGRADINGASSUBPASS) },
    { "customResolveAsSubpass",     MATERIAL(COLORGRADING, CUSTOMRESOLVEASSUBPASS) },
};

utils::Slice<const StaticMaterialInfo> getColorGradingMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace filament
