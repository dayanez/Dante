/*
 * Copyright (C) 2022 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GLTFIO_ARCHIVE_CACHE_H
#define GLTFIO_ARCHIVE_CACHE_H

#include <uberz/ReadableArchive.h>

#include <filament/Engine.h>
#include <filament/Material.h>

#include <utils/CString.h>
#include <utils/FixedCapacityVector.h>

#include <tsl/robin_map.h>

#include <string_view>

namespace filament::gltfio {

    struct ArchiveRequirements;
    using FeatureMap = tsl::robin_map<std::string_view, uberz::ArchiveFeature>;

    // Stores a set of Filament materials and knows how to choose a suitable material when given a
    // set of requirements. Used by gltfio; users do not need to access this class directly.
    class ArchiveCache {
    public:
        ArchiveCache(Engine& engine) : mEngine(engine) {}
        ~ArchiveCache();

        void load(const void* archiveData, uint64_t archiveByteCount);
        Material* getMaterial(const ArchiveRequirements& requirements);
        Material* getDefaultMaterial();
        const Material* const* getMaterials() const noexcept { return mMaterials.data(); }
        size_t getMaterialsCount() const noexcept { return mMaterials.size(); }
        void destroyMaterials();
        FeatureMap getFeatureMap(Material* material) const;

    private:
        Engine& mEngine;
        utils::FixedCapacityVector<Material*> mMaterials;
        uberz::ReadableArchive* mArchive = nullptr;
    };

    struct ArchiveRequirements {
        Shading shadingModel;
        BlendingMode blendingMode;
        tsl::robin_map<utils::CString, bool> features;
    };

} // namespace filament::uberz

#endif // GLTFIO_ARCHIVE_CACHE_H
