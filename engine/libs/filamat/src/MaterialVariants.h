/*
 * Copyright (C) 2019 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMAT_MATERIAL_VARIANTS_H
#define TNT_FILAMAT_MATERIAL_VARIANTS_H

#include <private/filament/Variant.h>

#include <backend/DriverEnums.h>

#include <vector>

namespace filamat {

struct Variant {
    using Stage = filament::backend::ShaderStage;
    Variant(filament::Variant v, Stage s) noexcept : variant(v), stage(s) {}
    filament::Variant variant;
    Stage stage;
};

std::vector<Variant> determineSurfaceVariants(
        filament::UserVariantFilterMask, bool isLit, bool shadowMultiplier);

std::vector<Variant> determinePostProcessVariants();

std::vector<Variant> determineComputeVariants();

} // namespace filamat

#endif
