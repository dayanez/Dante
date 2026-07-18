/*
 * Copyright (C) 2017 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "downcast.h"
#include "FNodeManager.h"

#include <utils/Log.h>

using namespace utils;

namespace filament::gltfio {

using Instance = NodeManager::Instance;

void FNodeManager::terminate() noexcept {
    auto& manager = mManager;
    if (!manager.empty()) {
#ifndef NDEBUG
        utils::slog.d << "cleaning up " << manager.getComponentCount()
            << " leaked node components" << utils::io::endl;
#endif
        while (!manager.empty()) {
            Instance ci = manager.end() - 1;
            manager.removeComponent(manager.getEntity(ci));
        }
    }
}

bool NodeManager::hasComponent(Entity e) const noexcept {
    return downcast(this)->hasComponent(e);
}

Instance NodeManager::getInstance(Entity e) const noexcept {
    return downcast(this)->getInstance(e);
}

void NodeManager::create(Entity entity) {
    downcast(this)->create(entity);
}

void NodeManager::destroy(Entity e) noexcept {
    downcast(this)->destroy(e);
}

void NodeManager::setMorphTargetNames(Instance ci, FixedCapacityVector<CString> names) noexcept {
    downcast(this)->setMorphTargetNames(ci, std::move(names));
}

const FixedCapacityVector<CString>& NodeManager::getMorphTargetNames(Instance ci) const noexcept {
    return downcast(this)->getMorphTargetNames(ci);
}

void NodeManager::setExtras(Instance ci, CString extras) noexcept {
    return downcast(this)->setExtras(ci, std::move(extras));
}

const CString& NodeManager::getExtras(Instance ci) const noexcept {
    return downcast(this)->getExtras(ci);
}

void NodeManager::setSceneMembership(Instance ci, SceneMask scenes) noexcept {
    downcast(this)->setSceneMembership(ci, scenes);
}

bitset32 NodeManager::getSceneMembership(Instance ci) const noexcept {
    return downcast(this)->getSceneMembership(ci);
}

} // namespace filament::gltfio
