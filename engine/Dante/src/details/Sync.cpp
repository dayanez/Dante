/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/Sync.h"

#include "details/Engine.h"

#include <filament/Sync.h>

#include <backend/Platform.h>

namespace filament {

using DriverApi = backend::DriverApi;

FSync::FSync(FEngine& engine)
    : mEngine(engine) {
    DriverApi& driverApi = engine.getDriverApi();
    mHwSync = driverApi.createSync();
}

void FSync::terminate(FEngine& engine) noexcept {
    engine.getDriverApi().destroySync(mHwSync);
}

void FSync::getExternalHandle(Sync::CallbackHandler* handler, Sync::Callback callback,
        void* userData) noexcept {
    mEngine.getDriverApi().getPlatformSync(mHwSync, handler, callback, userData);
}

} // namespace filament
