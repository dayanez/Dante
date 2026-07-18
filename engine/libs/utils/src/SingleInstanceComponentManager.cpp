/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <utils/Entity.h>
#include <utils/PagedArenaBitset.h>
#include <utils/SingleInstanceComponentManager.h>
#include <utils/Slice.h>

#include <algorithm>
#include <cstddef>
#include <utility>

namespace utils {

void SingleInstanceComponentManagerBase::registerChangeCallback(
        void const* token, ChangeCallback callback) noexcept {
    mChangeCallbacks.push_back({ token, std::move(callback) });
}

void SingleInstanceComponentManagerBase::unregisterChangeCallback(
        void const* token) noexcept {
    mChangeCallbacks.erase(
            std::remove_if(mChangeCallbacks.begin(), mChangeCallbacks.end(),
                    [token](auto const& info) { return info.token == token; }),
            mChangeCallbacks.end());
}

void SingleInstanceComponentManagerBase::notifyChange(Entity const e) noexcept {
    for (auto* bitset : mBitsets) {
        bitset->add(e.getId());
    }

    if constexpr (USE_SORTED_DIRTY_ARRAY) {
        auto const it = std::lower_bound(mDirtyEntities, mDirtyEntities + mDirtyCount, e);
        if (it != mDirtyEntities + mDirtyCount && *it == e) {
            return;
        }
        size_t const idx = it - mDirtyEntities;
        for (size_t i = mDirtyCount; i > idx; --i) {
            mDirtyEntities[i] = mDirtyEntities[i - 1];
        }
        mDirtyEntities[idx] = e;
        mDirtyCount++;
    } else {
        for (size_t i = 0; i < mDirtyCount; ++i) {
            if (mDirtyEntities[i] == e) {
                return;
            }
        }
        mDirtyEntities[mDirtyCount++] = e;
    }
    if (mDirtyCount == MAX_DIRTY_COUNT) {
        flushNotifications();
    }
}

void SingleInstanceComponentManagerBase::flushNotifications() noexcept {
    if (mDirtyCount > 0) {
        Slice<const Entity> const slice(mDirtyEntities, mDirtyCount);
        for (auto const& [token, callback] : mChangeCallbacks) {
            callback(slice);
        }
        mDirtyCount = 0;
    }
}

void SingleInstanceComponentManagerBase::registerBitset(PagedArenaBitset* bitset) {
    mBitsets.push_back(bitset);
}

void SingleInstanceComponentManagerBase::unregisterBitset(PagedArenaBitset const* bitset) {
    mBitsets.erase(std::remove(mBitsets.begin(), mBitsets.end(), bitset), mBitsets.end());
}
} // namespace utils
