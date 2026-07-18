/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <utils/compiler.h>
#include <utils/FixedCapacityVector.h>
#include <utils/Panic.h>

#include <stddef.h>
#include <stdlib.h>

namespace utils {

void FixedCapacityVectorBase::capacityCheckFailed(size_t const capacity, size_t const size) {
    UTILS_ASSUME(capacity < size);
    FILAMENT_CHECK_PRECONDITION(capacity >= size)
             << "capacity exceeded: requested size " << size
             << "u, available capacity " << capacity << "u.";

    // In practice, we will never reach this.
    abort();
}

} // namespace utils
