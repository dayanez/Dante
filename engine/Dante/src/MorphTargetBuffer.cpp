/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/MorphTargetBuffer.h"

#include "details/Engine.h"

namespace filament {

void MorphTargetBuffer::setPositionsAt(Engine& engine, size_t const targetIndex,
        math::float3 const* positions, size_t const count, size_t const offset) {
    downcast(this)->setPositionsAt(downcast(engine), targetIndex, positions, count, offset);
}

void MorphTargetBuffer::setPositionsAt(Engine& engine, size_t const targetIndex,
        math::float4 const* positions, size_t const count, size_t const offset) {
    downcast(this)->setPositionsAt(downcast(engine), targetIndex, positions, count, offset);
}

void MorphTargetBuffer::setTangentsAt(Engine& engine, size_t const targetIndex,
        math::short4 const* tangents, size_t const count, size_t const offset) {
    downcast(this)->setTangentsAt(downcast(engine), targetIndex, tangents, count, offset);
}

size_t MorphTargetBuffer::getVertexCount() const noexcept {
    return downcast(this)->getVertexCount();
}

size_t MorphTargetBuffer::getCount() const noexcept {
    return downcast(this)->getCount();
}

bool MorphTargetBuffer::hasPositions() const noexcept {
    return downcast(this)->hasPositions();
}

bool MorphTargetBuffer::hasTangents() const noexcept {
    return downcast(this)->hasTangents();
}

bool MorphTargetBuffer::isCustomMorphingEnabled() const noexcept {
    return downcast(this)->isCustomMorphingEnabled();
}

} // namespace filament
