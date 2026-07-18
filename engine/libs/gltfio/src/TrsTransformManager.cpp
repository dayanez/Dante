/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "downcast.h"
#include "FTrsTransformManager.h"

#include <gltfio/TrsTransformManager.h>

#include <utils/Log.h>

using namespace utils;

namespace filament::gltfio {

using Instance = TrsTransformManager::Instance;

void FTrsTransformManager::terminate() noexcept {
    auto& manager = mManager;
    if (!manager.empty()) {
#ifndef NDEBUG
        utils::slog.d << "cleaning up " << manager.getComponentCount()
            << " leaked trs transform components" << utils::io::endl;
#endif
        while (!manager.empty()) {
            Instance ci = manager.end() - 1;
            manager.removeComponent(manager.getEntity(ci));
        }
    }
}

bool TrsTransformManager::hasComponent(Entity e) const noexcept {
    return downcast(this)->hasComponent(e);
}

Instance TrsTransformManager::getInstance(Entity e) const noexcept {
    return downcast(this)->getInstance(e);
}

void TrsTransformManager::create(Entity entity) {
    downcast(this)->create(entity);
}

void TrsTransformManager::create(Entity entity, const float3& translation,
        const quatf& rotation, const float3& scale) {
    downcast(this)->create(entity, translation, rotation, scale);
}

void TrsTransformManager::destroy(Entity e) noexcept {
    downcast(this)->destroy(e);
}

void TrsTransformManager::setTranslation(Instance ci, const float3& translation) noexcept {
    downcast(this)->setTranslation(ci, translation);
}

const float3& TrsTransformManager::getTranslation(Instance ci) const noexcept {
    return downcast(this)->getTranslation(ci);
}

void TrsTransformManager::setRotation(Instance ci, const quatf& rotation) noexcept {
    downcast(this)->setRotation(ci, rotation);
}

const quatf& TrsTransformManager::getRotation(Instance ci) const noexcept {
    return downcast(this)->getRotation(ci);
}

void TrsTransformManager::setScale(Instance ci, const float3& scale) noexcept {
    downcast(this)->setScale(ci, scale);
}

const float3& TrsTransformManager::getScale(Instance ci) const noexcept {
    return downcast(this)->getScale(ci);
}

void TrsTransformManager::setTrs(Instance ci, const float3& translation,
        const quatf& rotation, const float3& scale) noexcept {
    downcast(this)->setTrs(ci, translation, rotation, scale);
}

const mat4f TrsTransformManager::getTransform(Instance ci) const noexcept {
    return downcast(this)->getTransform(ci);
}

} // namespace filament::gltfio
