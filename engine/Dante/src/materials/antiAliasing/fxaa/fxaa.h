/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

namespace filament {

utils::Slice<const StaticMaterialInfo> getFxaaMaterialList() noexcept;

} // namespace filament
