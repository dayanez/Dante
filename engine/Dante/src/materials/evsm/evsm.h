/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

namespace filament {

utils::Slice<const StaticMaterialInfo> getEvsmMaterialList() noexcept;

} // namespace filament
