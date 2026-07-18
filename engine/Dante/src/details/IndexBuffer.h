/*
 * Copyright (C) 2015 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_DETAILS_INDEXBUFFER_H
#define TNT_FILAMENT_DETAILS_INDEXBUFFER_H

#include "downcast.h"

#include <filament/IndexBuffer.h>

#include <backend/Handle.h>

#include <utils/compiler.h>

namespace filament {

class FEngine;

class FIndexBuffer : public IndexBuffer {
public:
    FIndexBuffer(FEngine& engine, const Builder& builder);

    // frees driver resources, object becomes invalid
    void terminate(FEngine& engine);

    backend::Handle<backend::HwIndexBuffer> getHwHandle() const noexcept { return mHandle; }

    size_t getIndexCount() const noexcept { return mIndexCount; }

    void setBuffer(FEngine& engine, BufferDescriptor&& buffer, uint32_t byteOffset = 0);

    AsyncCallId setBufferAsync(FEngine& engine, BufferDescriptor&& buffer, uint32_t byteOffset,
            backend::CallbackHandler* handler, AsyncCompletionCallback callback, void* user);

    bool isCreationComplete() const noexcept { return mCreationComplete.load(std::memory_order_relaxed); }

private:
    friend class IndexBuffer;
    backend::Handle<backend::HwIndexBuffer> mHandle;
    uint32_t mIndexCount;

    // This field is set to true when the creation process is complete. This is especially useful
    // asynchronous creation. If we can guarantee that this field is only referenced by the main
    // thread, we don't have to use atomic here.
    std::atomic_bool mCreationComplete{ false };
};

FILAMENT_DOWNCAST(IndexBuffer)

} // namespace filament

#endif // TNT_FILAMENT_DETAILS_INDEXBUFFER_H
