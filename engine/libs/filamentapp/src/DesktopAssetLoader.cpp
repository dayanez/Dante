/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <filamentapp/AssetLoader.h>
#include <filamentapp/DesktopAssetLoader.h>

#include <fstream>

namespace filament::app {

std::vector<uint8_t> DesktopAssetLoader::load(utils::Path const& path) const {
    std::ifstream in(path.c_str(), std::ifstream::binary | std::ifstream::ate);
    if (!in.is_open()) {
        return {};
    }

    auto size = in.tellg();
    if (size <= 0) {
        return {};
    }

    in.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    if (!in.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return {};
    }

    return buffer;
}

} // namespace filament::app
