/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "dof.h"
#include "generated/resources/dof.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace filament {

static const StaticMaterialInfo sMaterialList[] = {
        { "dof",                MATERIAL(DOF, DOF) },
        { "dofCoc",             MATERIAL(DOF, DOFCOC) },
        { "dofCombine",         MATERIAL(DOF, DOFCOMBINE) },
        { "dofDilate",          MATERIAL(DOF, DOFDILATE) },
        { "dofDownsample",      MATERIAL(DOF, DOFDOWNSAMPLE) },
        { "dofMedian",          MATERIAL(DOF, DOFMEDIAN) },
        { "dofMipmap",          MATERIAL(DOF, DOFMIPMAP) },
        { "dofTiles",           MATERIAL(DOF, DOFTILES) },
        { "dofTilesSwizzle",    MATERIAL(DOF, DOFTILESSWIZZLE) },
};

utils::Slice<const StaticMaterialInfo> getDofMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace filament
