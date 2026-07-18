/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/InstanceBuffer.h"

#include <filament/InstanceBuffer.h>

#include <math/mat4.h>

#include <cstddef>

namespace filament {

size_t InstanceBuffer::getInstanceCount() const noexcept {
    return downcast(this)->getInstanceCount();
}

void InstanceBuffer::setLocalTransforms(
        math::mat4f const* localTransforms, size_t const count, size_t const offset) {
    downcast(this)->setLocalTransforms(localTransforms, count, offset);
}

math::mat4f const& InstanceBuffer::getLocalTransform(size_t index) {
    return downcast(this)->getLocalTransform(index);
}

} // namespace filament
