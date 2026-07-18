/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENTAPP_DESKTOPASSETLOADER_H
#define TNT_FILAMENTAPP_DESKTOPASSETLOADER_H

#include <filamentapp/AssetLoader.h>

#include <utils/Path.h>

#include <cstdint>
#include <vector>

namespace filament::app {

class DesktopAssetLoader : public AssetLoader {
public:
    ~DesktopAssetLoader() override = default;

    std::vector<uint8_t> load(utils::Path const& path) const override;
};

} // namespace filament::app

#endif // TNT_FILAMENTAPP_DESKTOPASSETLOADER_H
