/*
 * Copyright (C) 2017 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_DETAILS_STREAM_H
#define TNT_FILAMENT_DETAILS_STREAM_H

#include "downcast.h"

#include <filament/Stream.h>

#include <backend/Handle.h>

#include <utils/compiler.h>

#include <math/mat3.h>

namespace filament {

class FEngine;

class FStream : public Stream {
public:
    FStream(FEngine& engine, const Builder& builder) noexcept;
    void terminate(FEngine& engine) noexcept;

    backend::Handle<backend::HwStream> getHandle() const noexcept { return mStreamHandle; }

    void setAcquiredImage(void* image, Callback callback, void* userdata, math::mat3f const& transform) noexcept;
    void setAcquiredImage(void* image, backend::CallbackHandler* handler, Callback callback, void* userdata, math::mat3f const& transform) noexcept;

    void setDimensions(uint32_t width, uint32_t height) noexcept;

    StreamType getStreamType() const noexcept { return mStreamType; }

    uint32_t getWidth() const noexcept { return mWidth; }

    uint32_t getHeight() const noexcept { return mHeight; }

    int64_t getTimestamp() const noexcept;

private:
    FEngine& mEngine;
    const StreamType mStreamType;
    backend::Handle<backend::HwStream> mStreamHandle;
    void* mNativeStream = nullptr;
    uint32_t mWidth;
    uint32_t mHeight;
};

FILAMENT_DOWNCAST(Stream)

} // namespace filament

#endif // TNT_FILAMENT_DETAILS_STREAM_H
