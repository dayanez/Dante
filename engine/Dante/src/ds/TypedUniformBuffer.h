/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_TYPEDUNIFORMBUFFER_H
#define TNT_FILAMENT_TYPEDUNIFORMBUFFER_H

#include "TypedBuffer.h"

#include <backend/BufferDescriptor.h>
#include <backend/DriverApiForward.h>
#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <utils/debug.h>

#include <utility>

#include <stddef.h>

namespace filament {

template<typename T, size_t N = 1>
class TypedUniformBuffer {
public:

    TypedUniformBuffer() noexcept = default;

    explicit TypedUniformBuffer(backend::DriverApi& driver) noexcept {
        init(driver);
    }

    void init(backend::DriverApi& driver) noexcept {
        assert_invariant(!mUboHandle);
        mUboHandle = driver.createBufferObject(
                mTypedBuffer.getSize(),
                backend::BufferObjectBinding::UNIFORM,
                backend::BufferUsage::DYNAMIC);
    }

    void terminate(backend::DriverApi& driver) noexcept {
        assert_invariant(mUboHandle);
        driver.destroyBufferObject(std::move(mUboHandle));
    }

    ~TypedUniformBuffer() noexcept {
        assert_invariant(!mUboHandle);
    }

    TypedBuffer<T,N>& getTypedBuffer() noexcept {
        return mTypedBuffer;
    }

    backend::BufferObjectHandle getUboHandle() const noexcept {
        return mUboHandle;
    }

    T& itemAt(size_t i) noexcept {
        return mTypedBuffer.itemAt(i);
    }

    T& edit() noexcept {
        return mTypedBuffer.itemAt(0);
    }

    // size of the uniform buffer in bytes
    size_t getSize() const noexcept { return mTypedBuffer.getSize(); }

    // return if any uniform has been changed
    bool isDirty() const noexcept { return mTypedBuffer.isDirty(); }

    // mark the whole buffer as "clean" (no modified uniforms)
    void clean() const noexcept { mTypedBuffer.clean(); }

    // helper functions
    backend::BufferDescriptor toBufferDescriptor(backend::DriverApi& driver) const noexcept {
        return mTypedBuffer.toBufferDescriptor(driver);
    }

private:
    TypedBuffer<T,N> mTypedBuffer;
    backend::BufferObjectHandle mUboHandle;
};

} // namespace filament


#endif //TNT_FILAMENT_TYPEDUNIFORMBUFFER_H
