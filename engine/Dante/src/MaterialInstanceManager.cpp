/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */


#include "MaterialInstanceManager.h"

#include "details/Engine.h"
#include "details/Material.h"

#include <cstdint>

namespace filament {

using namespace utils;

MaterialInstanceManager::MaterialInstanceManager() noexcept = default;

MaterialInstanceManager::MaterialInstanceManager(MaterialInstanceManager&& rhs) noexcept = default;
MaterialInstanceManager& MaterialInstanceManager::operator=(MaterialInstanceManager&& rhs) noexcept = default;
MaterialInstanceManager::~MaterialInstanceManager() = default;

void MaterialInstanceManager::terminate(FEngine& engine) {
    for (auto const& [key, instance] : mMaterialInstances) {
        engine.destroy(instance);
    }
    mMaterialInstances.clear();

    for (auto const& [material, pool] : mAnonymousMaterialInstances) {
        for (auto instance : pool.instances) {
            engine.destroy(instance);
        }
    }
    mAnonymousMaterialInstances.clear();
}

void MaterialInstanceManager::reset() {
    for (auto& [material, pool] : mAnonymousMaterialInstances) {
        pool.highWaterMark = 0;
    }
}

FMaterialInstance* MaterialInstanceManager::getMaterialInstance(FMaterial const* ma, uint32_t tag) const {
    Key const key{ma, tag};
    auto it = mMaterialInstances.find(key);
    if (it != mMaterialInstances.end()) {
        return it->second;
    }

    FMaterialInstance* const instance = ma->createInstance(ma->getName().c_str_safe());
    mMaterialInstances.emplace(key, instance);
    return instance;
}

FMaterialInstance* MaterialInstanceManager::getMaterialInstance(FMaterial const* ma) const {
    AnonymousPool& pool = mAnonymousMaterialInstances[ma];
    if (pool.highWaterMark < pool.instances.size()) {
        return pool.instances[pool.highWaterMark++];
    }

    FMaterialInstance* const instance = ma->createInstance(ma->getName().c_str_safe());
    pool.instances.push_back(instance);
    pool.highWaterMark++;
    return instance;
}

} // namespace filament
