/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/BufferObject.h"

#include "FilamentAPI-impl.h"

#include "details/Engine.h"

namespace filament {

void BufferObject::setBuffer(Engine& engine,
        BufferDescriptor&& buffer, uint32_t const byteOffset) {
    downcast(this)->setBuffer(downcast(engine), std::move(buffer), byteOffset);
}

size_t BufferObject::getByteCount() const noexcept {
    return downcast(this)->getByteCount();
}

} // namespace filament
