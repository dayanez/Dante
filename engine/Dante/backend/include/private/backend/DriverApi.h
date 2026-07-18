/*
 * Copyright (C) 2015 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_PRIVATE_DRIVERAPI_H
#define TNT_FILAMENT_BACKEND_PRIVATE_DRIVERAPI_H

#include <private/backend/CommandStream.h>

#include <backend/DriverApiForward.h>

#include <stddef.h>

namespace filament::backend {

inline void* allocateFromCommandStream(DriverApi& driver, size_t size, size_t alignment) noexcept {
    return driver.allocate(size, alignment);
}

} // namespace filament::backend

#endif // TNT_FILAMENT_BACKEND_PRIVATE_DRIVERAPI_H
