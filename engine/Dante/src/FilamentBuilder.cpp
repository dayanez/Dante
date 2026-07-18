/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <filament/FilamentAPI.h>

#include <utils/ImmutableCString.h>

#include <algorithm>

namespace filament {

void builderMakeName(utils::ImmutableCString& outName, const char* name, size_t const len) noexcept {
    if (!name) {
        return;
    }
    size_t const length = std::min(len, size_t { 128u });
    outName = utils::ImmutableCString(name, length);
}

} // namespace filament
