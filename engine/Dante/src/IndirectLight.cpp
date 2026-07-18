/*
 * Copyright (C) 2016 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/IndirectLight.h"

#include "details/Texture.h"

namespace filament {

using namespace math;

void IndirectLight::setIntensity(float const intensity) noexcept {
    downcast(this)->setIntensity(intensity);
}

float IndirectLight::getIntensity() const noexcept {
    return downcast(this)->getIntensity();
}

void IndirectLight::setRotation(mat3f const& rotation) noexcept {
    downcast(this)->setRotation(rotation);
}

const mat3f& IndirectLight::getRotation() const noexcept {
    return downcast(this)->getRotation();
}

Texture const* IndirectLight::getReflectionsTexture() const noexcept {
    return downcast(this)->getReflectionsTexture();
}

Texture const* IndirectLight::getIrradianceTexture() const noexcept {
    return downcast(this)->getIrradianceTexture();
}

float3 IndirectLight::getDirectionEstimate() const noexcept {
    return downcast(this)->getDirectionEstimate();
}

float4 IndirectLight::getColorEstimate(float3 const direction) const noexcept {
    return downcast(this)->getColorEstimate(direction);
}

float3 IndirectLight::getDirectionEstimate(const float3* sh) noexcept {
    return FIndirectLight::getDirectionEstimate(sh);
}

float4 IndirectLight::getColorEstimate(const float3* sh, float3 const direction) noexcept {
    return FIndirectLight::getColorEstimate(sh, direction);
}

} // namespace filament
