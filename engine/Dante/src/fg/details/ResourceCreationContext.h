/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <backend/DriverApiForward.h>

namespace filament {

class FrameGraph;
class TextureCacheInterface;

struct ResourceCreationContext {
    FrameGraph& fg;
    backend::DriverApi& driver;
    bool useProtectedMemory;
    TextureCacheInterface& getTextureCache() const noexcept;
};

} // namespace filament
