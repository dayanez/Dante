/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "evsm.h"
#include "generated/resources/evsm.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace filament {
static const StaticMaterialInfo sMaterialList[] = {
    {"vsmMipmap", MATERIAL(EVSM, VSMMIPMAP)},
    {"gaussian", MATERIAL(EVSM, GAUSSIAN)},
};

utils::Slice<const StaticMaterialInfo> getEvsmMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace filament
