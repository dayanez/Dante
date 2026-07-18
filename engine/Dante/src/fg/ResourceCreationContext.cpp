/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fg/details/ResourceCreationContext.h"

#include "FrameGraph.h"

namespace filament {

TextureCacheInterface& ResourceCreationContext::getTextureCache() const noexcept {
    return fg.getTextureCache();
}

} //namespace filament
