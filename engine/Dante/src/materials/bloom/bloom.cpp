/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bloom.h"
#include "generated/resources/bloom.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace filament {

static const StaticMaterialInfo sMaterialList[] = {
    { "bloomDownsample",            MATERIAL(BLOOM, BLOOMDOWNSAMPLE) },
    { "bloomDownsample2x",          MATERIAL(BLOOM, BLOOMDOWNSAMPLE2X) },
    { "bloomDownsample9",           MATERIAL(BLOOM, BLOOMDOWNSAMPLE9) },
    { "bloomUpsample",              MATERIAL(BLOOM, BLOOMUPSAMPLE) },
};

utils::Slice<const StaticMaterialInfo> getBloomMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace filament
