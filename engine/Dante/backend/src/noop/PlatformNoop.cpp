/*
 * Copyright (C) 2017 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "noop/PlatformNoop.h"

#include "noop/NoopDriver.h"

namespace filament::backend {

Driver* PlatformNoop::createDriver(void* sharedGLContext, const Platform::DriverConfig& driverConfig) {
    return NoopDriver::create();
}

} // namespace filament
