/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_SSRPASSDESCRIPTORSET_H
#define TNT_FILAMENT_SSRPASSDESCRIPTORSET_H

#include "DescriptorSet.h"
#include "TypedUniformBuffer.h"

#include <private/filament/UibStructs.h>

#include <backend/DriverApiForward.h>
#include <backend/Handle.h>

namespace filament {

class FEngine;

struct ScreenSpaceReflectionsOptions;

class SsrPassDescriptorSet {

    using TextureHandle = backend::Handle<backend::HwTexture>;

public:
    SsrPassDescriptorSet() noexcept;

    void init(FEngine& engine) noexcept;

    void terminate(backend::DriverApi& driver);

    void setFrameUniforms(FEngine const& engine, TypedUniformBuffer<PerViewUib>& uniforms) noexcept;

    void prepareStructure(FEngine const& engine, TextureHandle structure) noexcept;

    void prepareHistorySSR(FEngine const& engine, TextureHandle ssr) noexcept;

    // update local data into GPU UBO
    void commit(FEngine& engine) noexcept;

    // bind this descriptor set
    void bind(backend::DriverApi& driver) noexcept;

private:
    DescriptorSet mDescriptorSet;
    backend::BufferObjectHandle mShadowUbh;
};

} // namespace filament

#endif //TNT_FILAMENT_SSRPASSDESCRIPTORSET_H
