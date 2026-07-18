/*
 * Copyright (C) 2015 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/IndexBuffer.h"

#include "details/Engine.h"

namespace filament {

void IndexBuffer::setBuffer(Engine& engine,
        BufferDescriptor&& buffer, uint32_t const byteOffset) {
    downcast(this)->setBuffer(downcast(engine), std::move(buffer), byteOffset);
}

backend::AsyncCallId IndexBuffer::setBufferAsync(Engine& engine, BufferDescriptor&& buffer,
            uint32_t byteOffset, backend::CallbackHandler* handler,
            AsyncCompletionCallback callback, void* user) {
    return downcast(this)->setBufferAsync(downcast(engine), std::move(buffer), byteOffset,
            handler, std::move(callback), user);
}

size_t IndexBuffer::getIndexCount() const noexcept {
    return downcast(this)->getIndexCount();
}

bool IndexBuffer::isCreationComplete() const noexcept {
    return downcast(this)->isCreationComplete();
}

} // namespace filament
