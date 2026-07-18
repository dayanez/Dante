/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_PRIVATE_BLOBCACHEKEY_H
#define TNT_FILAMENT_BACKEND_PRIVATE_BLOBCACHEKEY_H

#include <backend/Program.h>

namespace filament::backend {

class BlobCacheKey {
public:
    using SpecializationConstants = utils::FixedCapacityVector<Program::SpecializationConstant>;

    BlobCacheKey() noexcept;
    BlobCacheKey(uint64_t id, SpecializationConstants const& specConstants);

    BlobCacheKey(BlobCacheKey const& rhs) = default;
    BlobCacheKey& operator=(BlobCacheKey const& rhs) = default;

    BlobCacheKey(BlobCacheKey&& rhs) noexcept;
    BlobCacheKey& operator=(BlobCacheKey&& rhs) noexcept;

    void const* data() const noexcept {
        return mData.get();
    }

    size_t size() const noexcept {
        return mSize;
    }

    explicit operator bool() const noexcept { return mSize > 0; }

    void swap(BlobCacheKey& other) noexcept {
        using std::swap;
        swap(other.mSize, mSize);
        swap(other.mData, mData);
    }

private:
    struct Key;

    friend void swap(BlobCacheKey& lhs, BlobCacheKey& rhs) noexcept {
        lhs.swap(rhs);
    }

    // we use a shared_ptr to play nice with std::function<>, it works because the buffer is const
    std::shared_ptr<Key> mData{};
    size_t mSize{};
};

} // namespace filament::backend

#endif // TNT_FILAMENT_BACKEND_PRIVATE_BLOBCACHEKEY_H
