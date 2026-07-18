/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "BlobCacheKey.h"

#include <memory>

namespace filament::backend {

struct BlobCacheKey::Key {
    uint64_t id;
    Program::SpecializationConstant constants[];
};

BlobCacheKey::BlobCacheKey() noexcept = default;

BlobCacheKey::BlobCacheKey(uint64_t id,
        BlobCacheKey::SpecializationConstants const& specConstants) {
    mSize = sizeof(Key) + sizeof(Key::constants[0]) * specConstants.size();

    Key* const pKey = (Key *)malloc(mSize);
    memset(pKey, 0, mSize);
    mData.reset(pKey, ::free);

    mData->id = id;
    for (size_t i = 0; i < specConstants.size(); i++) {
        mData->constants[i] = specConstants[i];
    }
}

BlobCacheKey::BlobCacheKey(BlobCacheKey&& rhs) noexcept
        : mData(std::move(rhs.mData)), mSize(rhs.mSize) {
    rhs.mSize = 0;
}

BlobCacheKey& BlobCacheKey::operator=(BlobCacheKey&& rhs) noexcept {
    if (this != &rhs) {
        using std::swap;
        swap(mData, rhs.mData);
        swap(mSize, rhs.mSize);
    }
    return *this;
}

} // namespace filament::backend
