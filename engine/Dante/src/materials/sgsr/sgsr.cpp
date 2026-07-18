/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "generated/resources/sgsr.h"

#include "fsr.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace filament {

static const StaticMaterialInfo sMaterialList[] = {
        { "sgsr1",                      MATERIAL(SGSR, SGSR1) },
};

utils::Slice<const StaticMaterialInfo> getSgsrMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace filament
