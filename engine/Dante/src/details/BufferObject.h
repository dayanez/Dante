/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_DETAILS_BUFFEROBJECT_H
#define TNT_FILAMENT_DETAILS_BUFFEROBJECT_H

#include "downcast.h"

#include <filament/BufferObject.h>

#include <backend/Handle.h>

#include <utils/compiler.h>

namespace filament {

class FEngine;

class FBufferObject : public BufferObject {
public:
    FBufferObject(FEngine& engine, const Builder& builder);

    // frees driver resources, object becomes invalid
    void terminate(FEngine& engine);

    backend::Handle<backend::HwBufferObject> getHwHandle() const noexcept { return mHandle; }

    size_t getByteCount() const noexcept { return mByteCount; }

    BindingType getBindingType() const noexcept { return mBindingType; }

private:
    friend class BufferObject;
    void setBuffer(FEngine& engine, BufferDescriptor&& buffer, uint32_t byteOffset = 0);
    backend::Handle<backend::HwBufferObject> mHandle;
    uint32_t mByteCount;
    BindingType mBindingType;
};

FILAMENT_DOWNCAST(BufferObject)

} // namespace filament

#endif // TNT_FILAMENT_DETAILS_BUFFEROBJECT_H
