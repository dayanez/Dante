/*
 * Copyright (C) 2017 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "EntityManagerImpl.h"

#include <utils/Entity.h>
#include <utils/EntityManager.h>
#include <utils/Mutex.h>
#include <utils/PagedArenaBitset.h>

#include <cassert>
#include <cstddef>
#include <mutex>
#include <new>
#include <utility>

namespace utils {

EntityManager::Listener::~Listener() noexcept = default;

EntityManager::EntityManager() = default;

EntityManager::~EntityManager() = default;

EntityManager& EntityManager::get() noexcept {
    // note: we leak the EntityManager because it's more important that it survives everything else
    // the leak is really not a problem because the process is terminating anyway.
    static EntityManagerImpl* instance = new(std::nothrow) EntityManagerImpl;
    return *instance;
}

void EntityManager::create(size_t const n, Entity* entities) {
    static_cast<EntityManagerImpl *>(this)->create(n, entities);
}

void EntityManager::destroy(size_t const n, Entity* entities) noexcept {
    static_cast<EntityManagerImpl *>(this)->destroy(n, entities);
}

void EntityManager::registerListener(Listener* l) noexcept {
    static_cast<EntityManagerImpl *>(this)->registerListener(l);
}

void EntityManager::unregisterListener(Listener* l) noexcept {
    static_cast<EntityManagerImpl *>(this)->unregisterListener(l);
}

void EntityManager::registerChangeCallback(void const* token, ChangeCallback callback) noexcept {
    static_cast<EntityManagerImpl *>(this)->registerChangeCallback(token, std::move(callback));
}

void EntityManager::unregisterChangeCallback(void const* token) noexcept {
    static_cast<EntityManagerImpl *>(this)->unregisterChangeCallback(token);
}

void EntityManager::flushNotifications() noexcept {
    static_cast<EntityManagerImpl *>(this)->flushNotifications();
}

size_t EntityManager::getEntityCount() const noexcept {
    return static_cast<EntityManagerImpl const *>(this)->getEntityCount();
}

#if FILAMENT_UTILS_TRACK_ENTITIES
std::vector<Entity> EntityManager::getActiveEntities() const {
    return static_cast<EntityManagerImpl const *>(this)->getActiveEntities();
}

void EntityManager::dumpActiveEntities(utils::io::ostream& out) const {
    static_cast<EntityManagerImpl const *>(this)->dumpActiveEntities(out);
}

#endif

bool EntityManager::isAlive(Entity const e) const noexcept {
    return static_cast<EntityManagerImpl const *>(this)->isAlive(e);
}

PagedArenaBitset EntityManager::getAliveEntities() const noexcept {
    auto const* impl = static_cast<EntityManagerImpl const*>(this);
    utils::LockGuard const lock(impl->mFreeListLock);
    return impl->mAliveEntities.clone();
}

} // namespace utils
