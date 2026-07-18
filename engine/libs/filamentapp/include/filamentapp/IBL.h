/*
 * Copyright (C) 2016 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_SAMPLE_IBL_H
#define TNT_FILAMENT_SAMPLE_IBL_H

#include <filament/Texture.h>

#include <math/vec3.h>

#include <sstream>
#include <string>

namespace filament {
class Engine;
class IndexBuffer;
class IndirectLight;
class Material;
class MaterialInstance;
class Renderable;
class Texture;
class Skybox;
}

namespace utils {
    class Path;
}

class IBL {
public:
    explicit IBL(filament::Engine& engine);
    ~IBL();

    bool loadFromEquirect(const utils::Path& path);
    bool loadFromDirectory(const utils::Path& path);
    bool loadFromKtx(const std::string& prefix);

    filament::IndirectLight* getIndirectLight() const noexcept {
        return mIndirectLight;
    }

    filament::Skybox* getSkybox() const noexcept {
        return mSkybox;
    }

    filament::Texture* getFogTexture() const noexcept {
        return mFogTexture;
    }

    bool hasSphericalHarmonics() const { return mHasSphericalHarmonics; }
    filament::math::float3 const* getSphericalHarmonics() const { return mBands; }

private:
    bool loadCubemapLevel(filament::Texture** texture, const utils::Path& path,
            size_t level = 0, std::string const& levelPrefix = "") const;


    bool loadCubemapLevel(filament::Texture** texture,
            filament::Texture::PixelBufferDescriptor* outBuffer,
            uint32_t* dim,
            const utils::Path& path,
            size_t level = 0, std::string const& levelPrefix = "") const;

    filament::Engine& mEngine;

    filament::math::float3 mBands[9] = {};
    bool mHasSphericalHarmonics = false;

    filament::Texture* mTexture = nullptr;
    filament::IndirectLight* mIndirectLight = nullptr;
    filament::Texture* mSkyboxTexture = nullptr;
    filament::Texture* mFogTexture = nullptr;
    filament::Skybox* mSkybox = nullptr;
};

#endif // TNT_FILAMENT_SAMPLE_IBL_H
