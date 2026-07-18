/*
 * Copyright (C) 2016 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/Fence.h"

namespace filament {

using namespace backend;

FenceStatus Fence::waitAndDestroy(Fence* fence, Mode const mode) {
    return FFence::waitAndDestroy(downcast(fence), mode);
}

FenceStatus Fence::wait(Mode const mode, uint64_t const timeout) {
    return downcast(this)->wait(mode, timeout);
}

} // namespace filament
