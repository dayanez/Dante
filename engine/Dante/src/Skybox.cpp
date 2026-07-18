/*
 * Copyright (C) 2016 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/Skybox.h"

#include "details/Texture.h"

namespace filament {

void Skybox::setLayerMask(uint8_t const select, uint8_t const values) noexcept {
    downcast(this)->setLayerMask(select, values);
}

uint8_t Skybox::getLayerMask() const noexcept {
    return downcast(this)->getLayerMask();
}

float Skybox::getIntensity() const noexcept {
    return downcast(this)->getIntensity();
}

void Skybox::setColor(math::float4 const color) noexcept {
    downcast(this)->setColor(color);
}

Texture const* Skybox::getTexture() const noexcept {
    return downcast(this)->getTexture();
}

} // namespace filament
