/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstdint>

namespace filament {

struct FrameGraphDummyLink {
    struct Descriptor {};
    struct SubResourceDescriptor {};
    using Usage = uint32_t;
    static constexpr Usage DEFAULT_R_USAGE = 0x1;
    static constexpr Usage DEFAULT_W_USAGE = 0x2;
};

} // namespace filament
