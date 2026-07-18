/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_TYPEDBUFFER_H
#define TNT_FILAMENT_TYPEDBUFFER_H

#include <private/backend/DriverApi.h>

#include <backend/BufferDescriptor.h>

#include <stddef.h>
#include <string.h>

namespace filament {

template<typename T, size_t N = 1>
class TypedBuffer { // NOLINT(cppcoreguidelines-pro-type-member-init)
public:

    T& itemAt(size_t i) noexcept {
        mSomethingDirty = true;
        return mBuffer[i];
    }

    T& edit() noexcept {
        return itemAt(0);
    }

    // size of the uniform buffer in bytes
    size_t getSize() const noexcept { return sizeof(T) * N; }

    // return if any uniform has been changed
    bool isDirty() const noexcept { return mSomethingDirty; }

    // mark the whole buffer as "clean" (no modified uniforms)
    void clean() const noexcept { mSomethingDirty = false; }

    // helper functions

    backend::BufferDescriptor toBufferDescriptor(backend::DriverApi& driver) const noexcept {
        return toBufferDescriptor(driver, 0, getSize());
    }

    // copy the UBO data and cleans the dirty bits
    backend::BufferDescriptor toBufferDescriptor(
            backend::DriverApi& driver, size_t offset, size_t size) const noexcept {
        backend::BufferDescriptor p;
        p.size = size;
        p.buffer = driver.allocate(p.size); // TODO: use out-of-line buffer if too large
        memcpy(p.buffer, reinterpret_cast<const char*>(mBuffer) + offset, p.size); // inlined
        clean();
        return p;
    }

private:
    T mBuffer[N];
    mutable bool mSomethingDirty = false;
};

} // namespace filament

#endif // TNT_FILAMENT_TYPEDBUFFER_H
