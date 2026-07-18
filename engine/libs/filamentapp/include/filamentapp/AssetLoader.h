/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENTAPP_ASSETLOADER_H
#define TNT_FILAMENTAPP_ASSETLOADER_H

#include <utils/Path.h>

#include <cstdint>
#include <vector>

namespace filament::app {

class AssetLoader {
public:
    virtual ~AssetLoader() = default;

    virtual std::vector<uint8_t> load(utils::Path const& path) const = 0;
};

} // namespace filament::app

#endif // TNT_FILAMENTAPP_ASSETLOADER_H
