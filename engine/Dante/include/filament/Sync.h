/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_SYNC_H
#define TNT_FILAMENT_SYNC_H

#include <filament/FilamentAPI.h>

#include <backend/CallbackHandler.h>
#include <backend/DriverEnums.h>
#include <backend/Platform.h>

namespace filament {

class UTILS_PUBLIC Sync : public FilamentAPI {
public:
    using CallbackHandler = backend::CallbackHandler;
    using Callback = backend::Platform::SyncCallback;

    /**
     * Fetches a handle to the external, platform-specific representation of
     * this sync object.
     *
     * @param handler A handler for the callback that will receive the handle
     * @param callback A callback that will receive the handle when ready
     * @param userData Data to be passed to the callback so that the application
     *                 can identify what frame the sync is relevant to.
     * @return The external handle for the Sync. This is valid destroy() is
     *         called on this Sync object.
     */
    void getExternalHandle(CallbackHandler* handler, Callback callback, void* userData) noexcept;

protected:
    // prevent heap allocation
    ~Sync() = default;
};

} // namespace filament

#endif // TNT_FILAMENT_SYNC_H
