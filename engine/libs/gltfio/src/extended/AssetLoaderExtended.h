/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GLTFIO_ASSETLOADEREXTENDED_H
#define GLTFIO_ASSETLOADEREXTENDED_H

#include "../FFilamentAsset.h"
#include "../Utility.h"

#include <gltfio/AssetLoader.h>

#include <backend/BufferDescriptor.h>

#include <cgltf.h>

#include <string>

namespace filament::gltfio {

struct Primitive;
struct FFilamentAsset;
class DracoCache;

using BufferDescriptor = filament::backend::BufferDescriptor;

// The cgltf attribute is a type and the attribute index
struct Attribute {
    cgltf_attribute_type type; // positions, tangents
    int index;
};

// The Filament Attribute is defined as a type and a slot.
struct FilamentAttribute {
    VertexAttribute attribute;
    int slot;
};

// AssetLoaderExtended performs the same task as AssetLoader. Specifically, it takes the data from
// cgltf and store them in CPU memory. These buffers are then forwarded to ResourceLoader for
// uplaoding to GPU. The difference between this class AssetLoader is that tangent space computation
// are computed here (and not in ResourceLoader). The reason for this change is that
// TangentSpaceMesh will remesh the input and possibly change the indices, vertex count, and
// triangle counts, and so those changes must be resolved before the buffers are sent to the GPU.
struct AssetLoaderExtended {
    using BufferSlot = FFilamentAsset::ResourceInfoExtended::BufferSlot;
    using Output = Primitive;

    struct Input {
        cgltf_data* gltf;
        cgltf_primitive* prim;
        char const* name;
        DracoCache* dracoCache;
        Material* material;
    };

    AssetLoaderExtended(AssetConfigurationExtended const& config, Engine* engine,
            MaterialProvider& materials)
        : mEngine(engine),
          mGltfPath(config.gltfPath),
          mMaterials(materials),
          mUriDataCache(std::make_shared<UriDataCache>()),
          mCgltfBuffersLoaded(false) {}

    ~AssetLoaderExtended() = default;

    bool createPrimitive(Input* input, Output* out, std::vector<BufferSlot>& outSlots);

    UriDataCacheHandle getUriDataCache() const noexcept { return mUriDataCache; }

private:
    filament::Engine* mEngine;
    std::string mGltfPath;
    MaterialProvider& mMaterials;
    UriDataCacheHandle mUriDataCache;
    bool mCgltfBuffersLoaded;
};

} // namespace filament::gltfio

#endif // GLTFIO_ASSETLOADEREXTENDED_H
