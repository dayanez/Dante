/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DynamicSpecConstKey.h"

#include <utils/Slice.h>

#include <array>
#include <cstdint>

namespace filament {

namespace {

constexpr auto get_keys() noexcept {
    std::array<DynamicSpecConstKey, DYNAMIC_SPEC_CONST_KEY_COUNT> keys;
    for (size_t i = 0; i < DYNAMIC_SPEC_CONST_KEY_COUNT; ++i) {
        keys[i] = DynamicSpecConstKey{uint16_t(i)};
    }
    return keys;
}

static auto const gDynamicSpecConstKeys{ get_keys() };

} // anonymous namespace

utils::Slice<const DynamicSpecConstKey> DynamicSpecConstKey::getAllPossibleKeys() noexcept {
    return { gDynamicSpecConstKeys.data(), gDynamicSpecConstKeys.size() };
}

} // namespace filament
